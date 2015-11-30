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

//Qt
#include <QtWidgets\qapplication.h>
#include <qdebug.h>

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
	vertexGrid = std::vector<Point>(_points[_currentCluster].begin(), _points[_currentCluster].end());

	std::sort(vertexGrid.begin(), vertexGrid.end());
	
	//Base 2-triangulation
	indexGrid.push_back(0);
	indexGrid.push_back(1);
	indexGrid.push_back(2);

	indexGrid.push_back(0);
	indexGrid.push_back(2);
	indexGrid.push_back(3);

}

void OpenGlWindow::clearCurrentPointAA(){
	_pointsAA[_currentCluster] = std::vector<Point>();
}

void OpenGlWindow::paintGrid()
{
	if (vertexGrid.size() == 0)
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

#pragma region Graham Scan

void OpenGlWindow::GrahamScan()
{
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
		CVector v = CVector(Point(0, 0), Point(1.0f,0));
		// Easier to debug like this
		float angle1 = v.angle(currentVec1);
		float angle2 = v.angle(currentVec2);
		// We check if we must take the inner or outer angle between the two vectors
		if (v.crossProduct(currentVec1)<0){
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
			prev = outPoints[outPoints.size()-1];
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
		if (isConvex(prevV,nextV)){
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
			}else{
				currIndex--;
			}
			//pivot = outPoints[currIndex];
			pivot = sInit;
			avance = false;
		}

	} while (pivot != sInit || avance == false);

	_pointsAA.at(_currentCluster).clear();
	//.clear();
	_pointsAA.insert(_pointsAA.begin() + _currentCluster, outPoints);
	_pointsAA[_currentCluster].push_back(outPoints[0]);
	//return outPoints;
}

bool OpenGlWindow::isConvex(CVector& v, CVector& v2) const{
	float angle = v.angle(v2);
	if (v.crossProduct(v2)<0){
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
	CVector v = CVector(Point(0,0),Point(0,-1.0f));
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

	std::cout << " Points cliques : " << outPoints.size() << " "  <<std::endl;
	printVector(outPoints);
	std::cout << " Points affiches : "  << polyPoints.size() << " " <<std::endl;
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
	if (model->mode == model->GRAHAMSCAN || model->mode == model->JARVIS){
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
