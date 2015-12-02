#define GLEW_STATIC
#include <GL/glew.h>
#include "OpenGlWindow.h"

#include <iostream>

// SOIL
#include <SOIL.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ESGIShader.h"

#include <vector>
#include "Point.h"
#include <math.h>
#include "CVector.h"
#include <functional>

//Qt
#include <QtWidgets\qapplication.h>
#include <qdebug.h>

#include "Circle.h"

#define PI 3.14159265

EsgiShader basicShader;
EsgiShader basicShader2;

EsgiShader grahanScanShader;


OpenGlWindow::OpenGlWindow(Model* model)
{
	this->model = model;
}


void OpenGlWindow::initializeGL()
{
	if (model->mode == model->GRAHAMSCAN)
	{
		initializePolygone();
	}

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, this->width(), this->height());

	glEnable(GL_DEPTH_TEST);

	basicShader.LoadVertexShader("basic.vs"); // vs or vert
	basicShader.LoadFragmentShader("basic.fs");
	basicShader.Create();

	basicShader2.LoadVertexShader("basic2.vs"); // vs or vert
	basicShader2.LoadFragmentShader("basic2.fs");
	basicShader2.Create();

	grahanScanShader.LoadVertexShader("GrahamScan.vs");
	grahanScanShader.LoadFragmentShader("GrahamScan.fs");
	grahanScanShader.Create();

	glEnable(GL_PROGRAM_POINT_SIZE);

	setMouseTracking(true);

}

void OpenGlWindow::initializePolygone()
{
	_movingPoint = Point();
	newCluster();
}

void OpenGlWindow::newCluster()
{
	_currentCluster++;
	_points.push_back(std::vector<Point>());
}

void OpenGlWindow::paintPoints(std::vector<float>& pointsF) const
{
	GLuint VAO = GLuint();
	GLuint VBO = GLuint();

	if (pointsF.size() >= 3){
		//pointsF.erase(pointsF.begin());
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, pointsF.size() * sizeof(float), &pointsF.front(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
	}


	if (pointsF.size() > 0){
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, pointsF.size() / 6);
		glBindVertexArray(0);
	}

}

void OpenGlWindow::paintGL()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (model->wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Activate shader
	//basicShader.Bind();


	std::vector<float> pointsF = std::vector<float>();
	if (model->mode == model->GRAHAMSCAN || model->mode == model->JARVIS)
	{

		grahanScanShader.Bind();

		if (model->mode == model->GRAHAMSCAN){
			GrahamScan();
			AddBaryCenter(pointsF);
		}
		else{
			JarvisMarch();
		}

		bool pointDrawing;

		// Si la structure de points apres algo est vide, alors on affiche les points cliqués en brut
		if (_pointsAA.size() == 0 || _pointsAA[_currentCluster].size() == 0){
			for (int i = 0; i < _points.size(); i++)
			{
				convertPointToFloat(_points[i], pointsF, pointColor);
				paintPoints(pointsF);
			}
		}
		// sinon on affiche les points de la structure de points après application de l'algo (jarvis, graham, etc)
		else{
			for (int i = 0; i < _pointsAA.size(); i++)
			{
				convertPointToFloat(_pointsAA[i], pointsF, pointColor);
				paintPoints(pointsF);
				paintLines(pointsF);
			}

			for (int i = 0; i < _points.size(); i++)
			{
				std::vector<float> pointsC = std::vector<float>();
				convertPointToFloat(_points[i], pointsC, pointColor2);
				paintPoints(pointsC);
			}
		}
		grahanScanShader.Unbind();
	}
	else if (model->mode == model->TRIANGULATION)
	{

		for (int i = 0; i < _points.size(); i++)
		{
			Triangulation();

			convertPointToFloat(_points[i], pointsF, pointColor);

			//paintPoints(pointsF);

			//paintLines(pointsF);
			paintGrid();
		}
	}

}

#pragma region Triangulation

void OpenGlWindow::Triangulation()
{
	_triangles.clear();
	_edges.clear();
	_vertex.clear();
	indexGrid.clear();
	_trianglesIndex.clear();
	_edgeToTriangle.clear();
	pointIndex = 0;

	if (_points[_currentCluster].size() < 3)
		return;

	vertexGrid = std::vector<Point>(_points[_currentCluster].begin(), _points[_currentCluster].end());

	std::sort(vertexGrid.begin(), vertexGrid.end());

	Vertex v1 = Vertex(vertexGrid[0]);
	Vertex v2 = Vertex(vertexGrid[1]);
	v1._index = pointIndex++;
	v2._index = pointIndex++;

	AddTriangleNew(v1, v2, vertexGrid[2], pointIndex++);


	if (vertexGrid.size() < 4)
		return;

	std::vector <std::function<void()>> jobs;

	for (int i = 3; i < vertexGrid.size(); i++)
	{
		Point newPoint = vertexGrid[i];

		for (int j = 0; j < _edges.size(); j++)
		{
			auto edge = _edges[j];

			//If it's an iterior edge, we can skip
			auto it = _edgeToTriangle.find(edge);
			if (it != _edgeToTriangle.end())
			{
				if (it->second.size() >= 2)
					continue;
			}
			else
			{
				continue;
			}

			//We need to choose an point that doesn't belong to the current edge 
			Point interiorP = interiorPoint(edge);
			CVector normalI = interiorNormal(edge, interiorP);
			CVector newEdge = CVector(edge._v1._coords, newPoint);

			float dotResult = normalI.dotProductMag(newEdge);
			bool visible = dotResult < 0;

			if (visible)
			{
				std::function<void()> fct = std::bind(&OpenGlWindow::AddTriangleNew, this, edge._v1, edge._v2, newPoint, pointIndex);
				jobs.push_back(fct);

				//AddTriangle(edge._v1, edge._v2, newPoint, pointIndex++);
			}
		}

		for each(auto fct in jobs)
		{
			fct();
		}

		pointIndex++;
		jobs.clear();
	}

	//Flipping Edge
	for (int j = 0; j < _edges.size(); j++)
	{
		auto edge = _edges[j];

		//If it's an exterior edge, we can skip
		auto it = _edgeToTriangle.find(edge);
		if (it != _edgeToTriangle.end())
		{
			if (it->second.size() == 1)
				continue;
		}
		else
		{
			continue;
		}


		bool toBeFlipped = !isDelaunay(it->second[0], it->second[1]);
		if (toBeFlipped)
		{
			std::vector<Vertex> tPoints = std::vector<Vertex>();
			Triangle t1 = it->second[0];
			Triangle t2 = it->second[1];

			findTrianglePoints(t1, t2, tPoints);

			_edges.erase(std::find(_edges.begin(), _edges.end(), edge));
			_edgeToTriangle.erase(_edgeToTriangle.find(edge));
			_trianglesIndex.erase(std::find(_trianglesIndex.begin(), _trianglesIndex.end(), t1));
			_trianglesIndex.erase(std::find(_trianglesIndex.begin(), _trianglesIndex.end(), t2));

			std::vector<Edge> tEdges = std::vector<Edge>();
			tEdges.push_back(t1._e1);
			tEdges.push_back(t1._e2);
			tEdges.push_back(t1._e3);
			tEdges.push_back(t2._e1);
			tEdges.push_back(t2._e2);
			tEdges.push_back(t2._e3);

			for (size_t i = 0; i < tEdges.size(); i++)
			{
				auto edgeC = tEdges[i];
				if (edgeC != edge)
				{
					auto itC = _edgeToTriangle.find(edgeC);
					if (itC->second[0] == t1 || itC->second[0] == t2){
						itC->second.erase(itC->second.begin());
						continue;
					}
					if (itC->second.size() > 0 && itC->second[1] == t1 || itC->second[1] == t2)
						itC->second.erase(itC->second.begin() + 1);

				}
			}
			
			tPoints.erase(std::find(tPoints.begin(), tPoints.end(), edge._v1));
			tPoints.erase(std::find(tPoints.begin(), tPoints.end(), edge._v2));

			AddTriangle(edge._v1, tPoints[0], tPoints[1]);
			AddTriangle(edge._v2, tPoints[0], tPoints[1]);

		}

	}


}

float length(Point p1, Point p2)
{
	return sqrt((p1.x_ - p2.x_) * (p1.x_ - p2.x_) + (p1.y_ - p2.y_) * (p1.y_ - p2.y_));
}

void OpenGlWindow::findTrianglePoints(const Triangle& t1, const Triangle& t2, std::vector<Vertex>& outVec) const
{
	outVec.push_back(t1._e1._v1);
	outVec.push_back(t1._e1._v2);
	outVec.push_back(t1._e2._v2);

	if (std::find(outVec.begin(), outVec.end(), t2._e1._v1) == outVec.end())
		outVec.push_back(t2._e1._v1);

	if (std::find(outVec.begin(), outVec.end(), t2._e1._v2) == outVec.end())
		outVec.push_back(t2._e1._v2);

	if (std::find(outVec.begin(), outVec.end(), t2._e2._v2) == outVec.end())
		outVec.push_back(t2._e2._v2);

}

bool OpenGlWindow::isDelaunay(Triangle t1, Triangle t2) const
{
	std::vector<Vertex> tPoints = std::vector<Vertex>();

	findTrianglePoints(t1, t2, tPoints);

	Circle c1 = Circle();
	c1.CalculateCircle(tPoints[0]._coords, tPoints[1]._coords, tPoints[2]._coords);

	if (length(c1._center, tPoints[3]._coords) < c1._radius)
		return false;
	else
		return true;
}

void OpenGlWindow::AddTriangleNew(Vertex v1, Vertex v2, Point p, int newIndex)
{
	Vertex v3 = Vertex(p);
	v3._index = newIndex;

	AddTriangle(v1, v2, v3);
}

void OpenGlWindow::AddTriangle(Vertex v1, Vertex v2, Vertex v3)
{
	Edge e1 = Edge(v1, v2);
	Edge e2 = Edge(v2, v3);
	Edge e3 = Edge(v3, v1);

	Triangle t = Triangle(e1, e2, e3, v1._index, v2._index, v3._index);

	_vertex.push_back(v1);
	_vertex.push_back(v2);
	_vertex.push_back(v2);

	if (std::find(_edges.begin(), _edges.end(), e1) == _edges.end())
		_edges.push_back(e1);
	if (std::find(_edges.begin(), _edges.end(), e2) == _edges.end())
		_edges.push_back(e2);
	if (std::find(_edges.begin(), _edges.end(), e3) == _edges.end())
		_edges.push_back(e3);

	_triangles.push_back(t);

	_trianglesIndex.push_back(t);

	std::vector<Triangle> vect1 = std::vector<Triangle>();
	vect1.push_back(t);

	auto it = _edgeToTriangle.find(e1);
	if (it != _edgeToTriangle.end())
		it->second.push_back(t);
	else
		_edgeToTriangle.insert(std::pair<Edge, std::vector<Triangle>>(e1, vect1));

	it = _edgeToTriangle.find(e2);
	if (it != _edgeToTriangle.end())
		it->second.push_back(t);
	else
	{
		std::vector<Triangle> vect2 = std::vector<Triangle>(vect1);
		_edgeToTriangle.insert(std::pair<Edge, std::vector<Triangle>>(e2, vect2));
	}

	it = _edgeToTriangle.find(e3);
	if (it != _edgeToTriangle.end())
		it->second.push_back(t);
	else
	{
		std::vector<Triangle> vect3 = std::vector<Triangle>(vect1);
		_edgeToTriangle.insert(std::pair<Edge, std::vector<Triangle>>(e3, vect3));
	}
}

Point OpenGlWindow::interiorPoint(Edge _currentEdge) const
{
	for (int j = 0; j < _edges.size(); j++)
	{
		auto edge = _edges[j];

		Vertex v1 = edge._v1;
		Vertex v2 = edge._v2;

		if (v1 != _currentEdge._v1 && v1 != _currentEdge._v2)
			return v1._coords;
		if (v2 != _currentEdge._v1 && v2 != _currentEdge._v2)
			return v2._coords;
	}
}

CVector OpenGlWindow::interiorNormal(const Edge& edge, const Point& point)  const
{
	Point p1 = edge._v1._coords;
	Point p2 = edge._v2._coords;

	float dX = p2.x_ - p1.x_;
	float dY = p2.y_ - p1.y_;

	//We chosse one normal
	CVector normal = CVector(-dY, dX);

	//We construct a vector using one point of the edge and another point from de structure
	CVector intVec = CVector(p1, point);

	//Is the normal the interior one?
	float result = normal.dotProductMag(intVec);

	if (result > 0)
		return normal;
	else
		return CVector(dY, -dX);
}

void OpenGlWindow::clearCurrentPointAA(){
	_pointsAA[_currentCluster] = std::vector<Point>();
}

void OpenGlWindow::feedIndexGrid()
{
	for (size_t i = 0; i < _trianglesIndex.size(); i++)
	{
		auto t = _trianglesIndex[i];

		indexGrid.push_back(t._index1);
		indexGrid.push_back(t._index2);
		indexGrid.push_back(t._index3);
	}
}

void OpenGlWindow::paintGrid()
{
	if (vertexGrid.size() == 0)
		return;
	std::vector<float> vertexF = std::vector<float>();

	convertPointToFloat(vertexGrid, vertexF, lineColor);
	feedIndexGrid();

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexF.size() * sizeof(float), &vertexF.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexGrid.size() * sizeof(float), &indexGrid.front(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAOs

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexGrid.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void OpenGlWindow::paintLines(std::vector<float>& pointsF) const
{
	GLuint VAO = GLuint();
	GLuint VBO = GLuint();

	if (pointsF.size() >= 3){
		//pointsF.erase(pointsF.begin());
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, pointsF.size() * sizeof(float), &pointsF.front(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
	}


	if (pointsF.size() > 0){
		glBindVertexArray(VAO);

		glDrawArrays(GL_LINE_STRIP, 0, pointsF.size() / 6);
		//glDrawArrays(GL_LINES, 0, pointsF.size() / 6);

		glBindVertexArray(0);
	}
}


#pragma endregion

#pragma region Graham Scan

void OpenGlWindow::GrahamScan()
{
	if (_points.size() == 0 || _points[_currentCluster].size() < 2){
		if (_pointsAA.size() < _points.size()){
			_pointsAA.push_back(std::vector<Point>());
		}
		return;
	}


	std::vector<Point> outPoints = std::vector<Point>(_points[_currentCluster].begin(), _points[_currentCluster].end());

	Point baryCenter = Point(0.0f, 0.0f, 0.0f);
	ComputeBaryCenter(outPoints, baryCenter);

	_baryCenter = baryCenter;
	int i = 0;

	// fonction de tri des points par ordre d'angle en utilisant une lambda
	std::sort(outPoints.begin(), outPoints.end(),
		[&baryCenter](const Point & a, const Point & b) -> bool
	{
		// vector BPj 
		CVector currentVec1 = CVector(baryCenter, a);
		// vector BPj+1
		CVector currentVec2 = CVector(baryCenter, b);
		// vector Ox
		CVector v = CVector(Point(0, 0), Point(1.0f, 0));
		// Easier to debug like this
		float angle1 = v.angle(currentVec1);
		float angle2 = v.angle(currentVec2);
		// We check if we must take the inner or outer angle between the two vectors
		if (v.crossProduct(currentVec1) < 0){
			angle1 = 2 * PI - angle1;
		}
		if (v.crossProduct(currentVec2) < 0){
			angle2 = 2 * PI - angle2;
		}
		//return v.angle(currentVec1) > v.angle(currentVec2);
		return angle1 < angle2;
	});

	std::cout << " Points triés par angle : " << outPoints.size() << " " << std::endl;
	printVector(outPoints);
}

void OpenGlWindow::ComputeBaryCenter(const std::vector<Point>& points, Point& baryCenter) const
{
	int size = points.size();

	for (size_t i = 0; i < size; i++)
	{
		baryCenter.x_ += points[i].x_;
		baryCenter.y_ += points[i].y_;
	}

	baryCenter.x_ /= size;
	baryCenter.y_ /= size;
}


#pragma endregion
void OpenGlWindow::AddBaryCenter(std::vector<float>& pointsF) const
{
	//pos
	pointsF.push_back(_baryCenter.x_);
	pointsF.push_back(_baryCenter.y_);
	pointsF.push_back(_baryCenter.z_);

	//color
	pointsF.push_back(baryCenterColor.x);
	pointsF.push_back(baryCenterColor.y);
	pointsF.push_back(baryCenterColor.z);
}

#pragma region Jarvis March

void OpenGlWindow::JarvisMarch()
{
	if (_points.size() == 0 || _points[_currentCluster].size() < 3){
		if (_pointsAA.size() < _points.size()){
			_pointsAA.push_back(std::vector<Point>());
		}
		return;
	}
	//std::cout << "Jarvis March" << std::endl;
	std::vector<Point> outPoints = std::vector<Point>(_points[_currentCluster].begin(), _points[_currentCluster].end());

	std::sort(outPoints.begin(), outPoints.end());
	CVector v = CVector(Point(0, 0), Point(0, -1.0f));
	std::vector<Point> polyPoints = std::vector<Point>();

	int indexFirst = 0;
	int i = indexFirst;
	int j;
	int inew;
	float angleMin;
	float distanceMax;
	float currentAngle;
	float currNorm;

	do{
		polyPoints.push_back(outPoints[i]);

		if (i == 0) j = 1;
		else j = 0;

		CVector firstVec = CVector(outPoints[i], outPoints[j]);
		angleMin = v.angle(firstVec);
		// si l'angle retourné n'est pas un alpha numérique, c'est parcequ'on a essayé de faire l'angle entre
		// le vecteur AB et le vecteur BA et donc ca ne va pas --> on remet la valeur min a 3 (valeur assez élevée)
		if (angleMin == -1 || isnan(angleMin)) { angleMin = 3.0f; };
		distanceMax = firstVec.norm();
		inew = j;
		for (j = inew + 1; j < outPoints.size(); j++){
			if (j != i){
				CVector currentVec = CVector(outPoints[i], outPoints[j]);
				currentAngle = v.angle(currentVec);

				currNorm = currentVec.norm();
				if (currentAngle < angleMin || (currentAngle == angleMin && distanceMax < currNorm)){
					angleMin = currentAngle;
					distanceMax = currNorm;
					inew = j;
				}
			}
		}
		v = CVector(outPoints[i], outPoints[inew]);
		i = inew;
	} while (indexFirst != i);

	std::cout << " Points cliques : " << outPoints.size() << " " << std::endl;
	printVector(outPoints);
	std::cout << " Points affiches : " << polyPoints.size() << " " << std::endl;
	int size = polyPoints.size();
	printVector(polyPoints);
	std::cout << "\n" << std::endl;
	//if (polyPoints.size() > 2)

	/*std::vector<Point> _pointsCurr = */
	_pointsAA.at(_currentCluster).clear();
	//.clear();
	_pointsAA.insert(_pointsAA.begin() + _currentCluster, polyPoints);
	_pointsAA[_currentCluster].push_back(polyPoints[0]);

	//else
	//_pointsAA.insert(_pointsAA.begin() + _currentCluster, _points[_currentCluster]);
}

#pragma endregion

#pragma region Utils
double convertViewportToOpenGLCoordinate(double x)
{
	return (x * 2) - 1;
}

void OpenGlWindow::mousePressEvent(QMouseEvent * event)
{
	if (model->mode == model->GRAHAMSCAN || model->mode == model->JARVIS || model->mode == model->TRIANGULATION){
		Point clickP = Point();
		clickP.x_ = convertViewportToOpenGLCoordinate(event->x() / (double)this->width());
		clickP.y_ = -convertViewportToOpenGLCoordinate(event->y() / (double)this->height());
		clickP.z_ = 0.0f;

		if (model->splineMode == model->CREATEPOINT)
		{
			_points[_currentCluster].push_back(clickP);
			this->update();
		}
		else if (model->splineMode == model->MOVEPOINT)
		{
			_hasClick = !_hasClick;
			if (_hasClick)
			{
				searchClosedPoint(clickP, _points, _movingPoint);
			}
		}

	}
}

void OpenGlWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (model->splineMode == model->MOVEPOINT && _hasClick)
	{
		_movingPoint.x_ = convertViewportToOpenGLCoordinate(event->x() / (double)this->width());
		_movingPoint.y_ = -convertViewportToOpenGLCoordinate(event->y() / (double)this->height());
		_movingPoint.z_ = 0.0f;
		this->update();
	}
}

void OpenGlWindow::searchClosedPoint(const Point click, const std::vector<std::vector<Point>>& points, Point& p) const
{
	for (int i = 0; i < points.size(); i++)
	{
		for (int j = 0; j < points[i].size(); j++)
		{
			p = points[i][j];
			if (click.isCloseTo(p, 0.05)){
				return;
			}
		}
	}

}

void OpenGlWindow::convertPointToFloat(const std::vector<Point>& points, std::vector<float>& pointsF, glm::vec3 color) const
{

	for (int i = 0; i < points.size(); i++)
	{
		pointsF.push_back(points[i].x_);
		pointsF.push_back(points[i].y_);
		pointsF.push_back(points[i].z_);
		pointsF.push_back(color.x);
		pointsF.push_back(color.y);
		pointsF.push_back(color.z);
	}
}


void OpenGlWindow::printVector(const std::vector<Point>& points) const
{
	for each (Point p in points)
	{
		std::cout << p << " -- ";
	}
	std::cout << std::endl;
}

void OpenGlWindow::clear()
{

	//_pointsAA[_currentCluster].clear();
	_points.clear();
	_pointsAA.clear();
	_currentCluster = 0;
	repaint();

	_points.push_back(std::vector<Point>());
	_pointsAA.push_back(std::vector<Point>());
}


#pragma endregion
