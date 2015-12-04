#define GLEW_STATIC
#include <GL/glew.h>
#include "OpenGlWindow.h"

#include <iostream>

#include <chrono>

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
	// Tips for time calculation
	//
	// function body

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (model->wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Activate shader
	//basicShader.Bind();


	//lines
	std::vector<float> pointsF = std::vector<float>();
	//points
	std::vector<float> pointsC = std::vector<float>();

	_points[0].clear();
	for (size_t i = 0; i < 100; i++)
	{
		for (size_t j = 0; j < 100; j++)
		{
			_points[0].push_back(Point(i * (1.0f / 32.0f), j * (1.0f / 32.0f)));
		}
	}

	if (model->mode == model->GRAHAMSCAN || model->mode == model->JARVIS)
	{

		grahanScanShader.Bind();

		if (model->mode == model->GRAHAMSCAN){
			GrahamScan();
			AddBaryCenter(pointsC);
		}
		else{
			JarvisMarch();
		}

		bool pointDrawing;

		// Si la structure de points apres algo est vide, alors on affiche les points cliqués en brut
		if (_pointsAA.size() == 0 /* || _pointsAA[_currentCluster].size() == 0*/){
			for (int i = 0; i < _points.size(); i++)
			{
				pointsF = std::vector<float>();
				convertPointToFloat(_points[i], pointsF, pointColor);
				paintPoints(pointsF);
			}
		}
		// sinon on affiche les points de la structure de points après application de l'algo (jarvis, graham, etc)
		else{
			for (int i = 0; i < _pointsAA.size(); i++)
			{
				//if (_pointsAA[_currentCluster].size() != 0){
					pointsF = std::vector<float>();		
					convertPointToFloat(_pointsAA[i], pointsF, pointColor);
					paintPoints(pointsF);
					paintLines(pointsF);
				//}
			}

			for (int i = 0; i < _points.size(); i++)
			{
				pointsC = std::vector<float>();
				convertPointToFloat(_points[i], pointsC, pointColor2);
				if (model->mode == model->GRAHAMSCAN){
					AddBaryCenter(pointsC);
				}
				paintPoints(pointsC);
			}
		}
		grahanScanShader.Unbind();
	}
	else if (model->mode == model->TRIANGULATION || model->mode == model->FLIPPING)
	{

		//for (int i = 0; i < _points.size(); i++)
		//{
		Triangulation(model->mode == model->FLIPPING);

		//convertPointToFloat(_points[i], pointsF, pointColor);

		//paintPoints(pointsF);

		//paintLines(pointsF);
		paintGrid();
		//}
	}
	else if (model->mode == model->VORONOI)
	{
		grahanScanShader.Bind();

		voronoi();

		if (model->wireFrame)
			paintGrid();
		paintVoronoi();

		grahanScanShader.Unbind();
	}

}

#pragma region Triangulation

void OpenGlWindow::Triangulation(bool flipping)
{
	_triangles.clear();
	_edges.clear();
	_vertex.clear();
	indexGrid.clear();
	_trianglesIndex.clear();
	_edgeToTriangle.clear();
	pointIndex = 0;

	if (_points.size() == 0 || _points[_currentCluster].size() < 3)
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

			}
		}

		for each(auto fct in jobs)
		{
			fct();
		}

		pointIndex++;
		jobs.clear();
	}

	if (!flipping)
		return;

	std::vector<Edge> edgeToCheck = std::vector<Edge>(_edges.begin(), _edges.end());

	//Flipping Edge
	while (edgeToCheck.size() > 0)
	{
		auto edge = edgeToCheck[0];
		edgeToCheck.erase(std::find(edgeToCheck.begin(), edgeToCheck.end(), edge));

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

			if (std::find(_edges.begin(), _edges.end(), edge) != _edges.end())
				_edges.erase(std::find(_edges.begin(), _edges.end(), edge));
			_edgeToTriangle.erase(_edgeToTriangle.find(edge));
			if (std::find(_trianglesIndex.begin(), _trianglesIndex.end(), t1) != _trianglesIndex.end())
				_trianglesIndex.erase(std::find(_trianglesIndex.begin(), _trianglesIndex.end(), t1));
			if (std::find(_trianglesIndex.begin(), _trianglesIndex.end(), t2) != _trianglesIndex.end())
				_trianglesIndex.erase(std::find(_trianglesIndex.begin(), _trianglesIndex.end(), t2));
			if (std::find(_triangles.begin(), _triangles.end(), t1) != _triangles.end())
				_triangles.erase(std::find(_triangles.begin(), _triangles.end(), t1));
			if (std::find(_triangles.begin(), _triangles.end(), t2) != _triangles.end())
				_triangles.erase(std::find(_triangles.begin(), _triangles.end(), t2));

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
					edgeToCheck.push_back(edgeC);
					auto itC = _edgeToTriangle.find(edgeC);
					if (itC == _edgeToTriangle.end())
						continue;

					if (itC->second[0] == t1 || itC->second[0] == t2){
						itC->second.erase(itC->second.begin());
						continue;
					}
					if (itC->second.size() > 0 && itC->second[1] == t1 || itC->second[1] == t2)
						itC->second.erase(itC->second.begin() + 1);

				}
			}

			if (std::find(tPoints.begin(), tPoints.end(), edge._v1) != tPoints.end())
				tPoints.erase(std::find(tPoints.begin(), tPoints.end(), edge._v1));
			if (std::find(tPoints.begin(), tPoints.end(), edge._v2) != tPoints.end())
				tPoints.erase(std::find(tPoints.begin(), tPoints.end(), edge._v2));

			AddTriangle(edge._v1, tPoints[0], tPoints[1]);
			AddTriangle(edge._v2, tPoints[0], tPoints[1]);

			edgeToCheck.push_back(Edge(tPoints[0], tPoints[1]));

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

	if (tPoints.size() < 4)
		return true;

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
	feedIndexGrid();
	if (vertexGrid.size() == 0 || indexGrid.size() == 0)
		return;
	std::vector<float> vertexF = std::vector<float>();

	convertPointToFloat(vertexGrid, vertexF, lineColor);

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

#pragma region Voronoi

CVector normal(const Point& p1, const Point& p2)
{
	float dX = p2.x_ - p1.x_;
	float dY = p2.y_ - p1.y_;

	//We chosse one normal
	return CVector(-dY, dX);
}

void OpenGlWindow::voronoi()
{
	Triangulation(true);
	_centers.clear();
	_indexCenters.clear();

	for (int j = 0; j < _edges.size(); j++)
	{
		auto edge = _edges[j];

		//If it's an exterior edge, we can skip
		auto it = _edgeToTriangle.find(edge);
		if (it->second.size() == 0)
			continue;
		if (it->second.size() == 1)
		{
			Triangle t1 = it->second[0];
			Circle c1 = Circle();
			c1.CalculateCircle(t1._e1._v1._coords, t1._e1._v2._coords, t1._e2._v2._coords);

			Point interiorP = interiorPoint(edge);
			CVector normalI = interiorNormal(edge, interiorP);
			normalI.x *= -1;
			normalI.y *= -1;

			Point p;
			p.x_ = c1._center.x_ + (normalI.x * 10);
			p.y_ = c1._center.y_ + (normalI.y * 10);

			auto itFind1 = std::find(_centers.begin(), _centers.end(), c1._center);

			if (itFind1 == _centers.end())
			{
				_indexCenters.push_back(_centers.size());
				_centers.push_back(c1._center);
			}
			else
			{
				_indexCenters.push_back(itFind1 - _centers.begin());
			}

			_centers.push_back(p);
			_indexCenters.push_back(_centers.size() - 1);

		}
		else
		{

			Triangle t1 = it->second[0];
			Triangle t2 = it->second[1];

			Circle c1 = Circle();
			c1.CalculateCircle(t1._e1._v1._coords, t1._e1._v2._coords, t1._e2._v2._coords);

			Circle c2 = Circle();
			c2.CalculateCircle(t2._e1._v1._coords, t2._e1._v2._coords, t2._e2._v2._coords);

			auto itFind1 = std::find(_centers.begin(), _centers.end(), c1._center);

			if (itFind1 == _centers.end())
			{
				_indexCenters.push_back(_centers.size());
				_centers.push_back(c1._center);
			}
			else
			{
				_indexCenters.push_back(itFind1 - _centers.begin());
			}

			auto itFind2 = std::find(_centers.begin(), _centers.end(), c2._center);

			if (itFind2 == _centers.end())
			{
				_indexCenters.push_back(_centers.size());
				_centers.push_back(c2._center);
			}
			else
			{
				_indexCenters.push_back(itFind2 - _centers.begin());
			}

		}
	}
}

void OpenGlWindow::paintVoronoi()
{
	if (_centers.size() == 0 || _indexCenters.size() == 0)
		return;

	GLuint VBO, VAO, EBO;

	std::vector<float> centersVertex = std::vector<float>();


	convertPointToFloat(_centers, centersVertex, voronoiColor);

	if (centersVertex.size() == 0)
		return;
	//pointsF.erase(pointsF.begin());
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, centersVertex.size() * sizeof(float), &centersVertex.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCenters.size() * sizeof(float), &_indexCenters.front(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glBindVertexArray(VAO);

	glDrawElements(GL_LINES, _indexCenters.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_POINTS, 0, centersVertex.size() / 6);
	//glDrawArrays(GL_LINES, 0, pointsF.size() / 6);

	glBindVertexArray(0);
}

#pragma endregion

#pragma region Graham Scan

void OpenGlWindow::GrahamScan()
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now(); //<= at the beginning of the function
	if (_points.size() == 0 || _points[_currentCluster].size() < 3){
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

	//std::cout << " Points triés par angle : " << outPoints.size() << " " << std::endl;
	//printVector(outPoints);
	Point sInit = outPoints[0];
	Point pivot = sInit;
	int currIndex = 0;
	bool prevIsLast = true;
	bool avance = false;

	do{
		pivot = outPoints[currIndex];
		Point prev;
		if (currIndex == 0){
			prev = outPoints[outPoints.size() - 1];
			prevIsLast = true;
		}
		else
		{
			prev = outPoints[currIndex - 1];
			prevIsLast = false;
		}
		Point next;
		if (currIndex == outPoints.size() - 1){
			next = outPoints[0];
		}
		else{
			next = outPoints[currIndex + 1];
		}

		CVector prevV = CVector(prev, pivot);
		CVector nextV = CVector(pivot, next);
		// pivot != next pour enlever les doublons
		if (isConvex(prevV, nextV) && pivot != next){
			pivot = next;
			avance = true;
			if (currIndex == outPoints.size() - 1){
				currIndex = 0;
			}
			else{
				currIndex++;
			}
		}
		else{
			// if this is the first element, we must update the new first elem
			sInit = prev;
			outPoints.erase(outPoints.begin() + currIndex);
			if (prevIsLast){
				// last-1 because it will pass in the increment after this line so it will be equal to last in the end
				// -2 because size if size is 5 then last element index is 4
				currIndex = outPoints.size() - 1;
			}
			else{
				currIndex--;
			}
			//pivot = outPoints[currIndex];
			pivot = sInit;
			avance = false;
		}

	} while (pivot != sInit || avance == false);

	if (_pointsAA.size() > 0)
		_pointsAA.at(_currentCluster).clear();
	//.clear();
	_pointsAA.insert(_pointsAA.begin() + _currentCluster, outPoints);
	_pointsAA[_currentCluster].push_back(outPoints[0]);
	//return outPoints;

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	float duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();  //     <= at the end of the function
	std::cout << duration << std::endl;
}

bool OpenGlWindow::isConvex(CVector& v, CVector& v2) const{
	float angle = v.angle(v2);
	if (v.crossProduct(v2) < 0){
		angle = 2 * PI - angle;
	}
	if (angle > PI){
		return false;
	}
	else{
		return true;
	}
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
#pragma endregion

#pragma region Jarvis March

void OpenGlWindow::JarvisMarch()
{
	if (_points.size() == 0 || _points[_currentCluster].size() < 3){
		if (_pointsAA.size() < _points.size()){
			_pointsAA.push_back(std::vector<Point>());
		}
		return;
	}
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now(); //<= at the beginning of the function

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

	//std::cout << " Points cliques : " << outPoints.size() << " " << std::endl;
	//printVector(outPoints);
	//std::cout << " Points affiches : " << polyPoints.size() << " " << std::endl;
	int size = polyPoints.size();
	//printVector(polyPoints);
	//std::cout << "\n" << std::endl;
	//if (polyPoints.size() > 2)

	/*std::vector<Point> _pointsCurr = */
	_pointsAA.at(_currentCluster).clear();
	//.clear();
	_pointsAA.insert(_pointsAA.begin() + _currentCluster, polyPoints);
	_pointsAA[_currentCluster].push_back(polyPoints[0]);

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	float duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();  //     <= at the end of the function
	std::cout << duration << std::endl;

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
	if (model->mode == model->GRAHAMSCAN || model->mode == model->JARVIS || model->mode == model->TRIANGULATION || model->mode == model->FLIPPING || model->mode == model->VORONOI){
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
				searchClosedPoint(clickP, _points);
			}
			this->update();
		}

	}
}

void OpenGlWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (model->splineMode == model->MOVEPOINT && _hasClick)
	{
		if (_movingPointPtr != nullptr){
			//std::cout << "MovingPoint Begin TONPAYRE : " << _movingPoint.x_ << " " << _movingPoint.y_ << std::endl;
			(*_movingPointPtr).x_ = convertViewportToOpenGLCoordinate(event->x() / (double)this->width());
			(*_movingPointPtr).y_ = -convertViewportToOpenGLCoordinate(event->y() / (double)this->height());
			(*_movingPointPtr).z_ = 0.0f;
			//std::cout << "MovingPoint End TONPAYRE: " << (*_movingPointPtr).x_ << " " << (*_movingPointPtr).y_ << std::endl;
			//std::cout << "MovingPoint Points TONPAYRE: " << _points[_currentCluster][0].x_ << " " << _points[_currentCluster][0].y_ << std::endl;
			this->update();
		}
	}
}

void OpenGlWindow::searchClosedPoint(const Point click, std::vector<std::vector<Point>>& points)
{
	for (int i = 0; i < points.size(); i++)
	{
		for (int j = 0; j < points[i].size(); j++)
		{
			Point *	p = &(points[i][j]);
			if (click.isCloseTo(*p, 0.05)){	
				_movingPointPtr = p;
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
		pointsF.push_back(0);
		pointsF.push_back(color.x);
		pointsF.push_back(color.y);
		pointsF.push_back(color.z);
	}
}


//void OpenGlWindow::intersection(const Point& a, const Point& b, const Point& c, const Point& d, Point& intersec) const
//{
//	//matrice 1, matrice inverse.
//	float matrixA[2][2];
//	float matrixAReverse[2][2];
//	float matrixRes[2];
//	float matrixB[2];
//	float det;
//
//	float x1 = a.x_get();
//	float x2 = b.x_get();
//	float x3 = c.x_get();
//	float x4 = d.x_get();
//
//	float y1 = a.y_get();
//	float y2 = b.y_get();
//	float y3 = c.y_get();
//	float y4 = d.y_get();
//
//	matrixA[0][0] = (x2 - x1);
//	matrixA[0][1] = (x3 - x4);
//	matrixA[1][0] = (y2 - y1);
//	matrixA[1][1] = (y3 - y4);
//
//	matrixB[0] = (c.x_get() - a.x_get());
//	matrixB[1] = (c.y_get() - a.y_get());
//
//	det = determinant(matrixA);
//
//	if (det == 0)
//		throw 1;
//
//	//Res = A-1 * B
//
//	//The Matrix A must be reversed
//	matrixAReverse[0][0] = matrixA[1][1] / det;
//	matrixAReverse[0][1] = -matrixA[0][1] / det;
//	matrixAReverse[1][0] = -matrixA[1][0] / det;
//	matrixAReverse[1][1] = matrixA[0][0] / det;
//
//	//Matrix product between reverse A and B matrix
//	matrixRes[0] = matrixAReverse[0][0] * matrixB[0] + matrixAReverse[0][1] * matrixB[1];
//	matrixRes[1] = matrixAReverse[1][0] * matrixB[0] + matrixAReverse[1][1] * matrixB[1];
//
//	//The intersection is outise of the polygon current line segment
//	//if (matrixRes[0] > 1 || matrixRes[0] < 0)
//	//	throw 2;
//
//	//Calculate the intersection point
//	intersec.x_set(((1 - matrixRes[0]) * x1) + (matrixRes[0] * x2));
//	intersec.y_set(((1 - matrixRes[0]) * y1) + (matrixRes[0] * y2));
//
//}


bool OpenGlWindow::intersection(const Point& sA, const Point& sB, const Point& dA, const Point& dB, Point& inter) const
{
	// Equation paramétrique d'une droite à partir de deux points
	// P(t) = sA + (sB - sA)t
	// Q(s) = dA + (dB - dA)s
	// ^ * X = b 

	// Définition de la matrice 2x2 -> ^
	float matrix[2][2];
	matrix[0][0] = sB.x_ - sA.x_;
	matrix[0][1] = dA.x_ - dB.x_;
	matrix[1][0] = sB.y_ - sA.y_;
	matrix[1][1] = dA.y_ - dB.y_;

	// Calcul du déterminant
	float determinant = (sB.x_ - sA.x_) * (dA.y_ - dB.y_) - (sB.y_ - sA.y_) * (dA.x_ - dB.x_);

	// On quitte si le déterminant est nul
	if (determinant == 0.0f)
		return false;

	// On calcul l'inverse de la matrice -> ^-1
	float invmatrix[2][2];
	invmatrix[0][0] = matrix[1][1] * (1 / determinant);
	invmatrix[0][1] = -matrix[0][1] * (1 / determinant);
	invmatrix[1][0] = -matrix[1][0] * (1 / determinant);
	invmatrix[1][1] = matrix[0][0] * (1 / determinant);

	// Définition du b
	float bMatrix[2];
	bMatrix[0] = dA.x_ - sA.x_;
	bMatrix[1] = dA.y_ - sA.y_;

	// Résultat de la multiplication -> ^-1 * b
	float X[2];
	// correspond à t
	X[0] = invmatrix[0][0] * bMatrix[0] + invmatrix[0][1] * bMatrix[1];

	// correspond à s
	X[1] = invmatrix[1][0] * bMatrix[0] + invmatrix[1][1] * bMatrix[1];

	inter = sA + (sB - sA) * X[0];
	return true;
}

float OpenGlWindow::determinant(float matrix[2][2]) const
{
	return (matrix[0][0] * matrix[1][1]) - (matrix[0][1] * matrix[1][0]);
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

void OpenGlWindow::resizeGL(int width, int height)
{
	//glViewport(0, 0, width, height);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();


}

#pragma endregion
