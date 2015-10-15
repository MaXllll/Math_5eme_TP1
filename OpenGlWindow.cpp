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

//Qt
#include <QtWidgets\qapplication.h>
#include <qdebug.h>

#define PI 3.14159265

EsgiShader basicShader;
EsgiShader basicShader2;

EsgiShader bsplineShader;


OpenGlWindow::OpenGlWindow(Model* model)
{
	this->model = model;
}


void OpenGlWindow::paintGrid()
{

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, verticesGrid.size() * sizeof(float), &verticesGrid.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesGrid.size() * sizeof(float), &indicesGrid.front(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // Unbind VAO

	basicShader.Bind();

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	model = glm::rotate(model, /*(GLfloat)glfwGetTime() **/ 1.f, glm::vec3(1.f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3(-0.5f, 0.8f, -2.0f));
	projection = glm::perspective(45.0f, (GLfloat)this->width() / (GLfloat)this->height(), 0.1f, 100.0f);
	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(basicShader.GetProgram(), "model");
	GLint viewLoc = glGetUniformLocation(basicShader.GetProgram(), "view");
	GLint projLoc = glGetUniformLocation(basicShader.GetProgram(), "projection");
	// Pass them to the shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, indicesGrid.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	basicShader.Unbind();
}

void OpenGlWindow::initializeGL()
{
	if (model->mode == model->BSPLINE)
	{
		initializePolygone();
	}
	else if (model->mode == model->EXTRUSION)
	{

	}
	else if (model->mode == model->BSURFACE)
	{
		
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

	bsplineShader.LoadVertexShader("BSpline.vs");
	bsplineShader.LoadFragmentShader("BSpline.fs");
	bsplineShader.Create();

	glEnable(GL_PROGRAM_POINT_SIZE);

	setMouseTracking(true);

}

void OpenGlWindow::initializePolygone()
{
	_movingPoint = Point();
	newPolygone();
}

void OpenGlWindow::newPolygone()
{
	_currentPolygone++;
	_points.push_back(std::vector<Point>());
}

void OpenGlWindow::paintPoints()
{
	bsplineShader.Bind();

	for (int i = 0; i < _points.size(); i++){

		std::vector<float> pointsF = std::vector<float>();
		GLuint VAO = GLuint();
		GLuint VBO = GLuint();

		convertPointToFloat(_points[i], pointsF);

		if (pointsF.size() > 3){
			//pointsF.erase(pointsF.begin());
			glGenVertexArrays(i + 1, &VAO);
			glGenBuffers(i + 1, &VBO);
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, pointsF.size() * sizeof(float), &pointsF.front(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
		}


		if (pointsF.size() > 0){
			glBindVertexArray(VAO);
			glDrawArrays(GL_POINTS, 0, pointsF.size() / 3);
			glBindVertexArray(0);
		}

	}
	bsplineShader.Unbind();
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
	basicShader.Bind();

	if (model->mode == model->BSPLINE)
	{
		paintPoints();
	}
	//else if (model->mode == model->EXTRUSION)
	//{
	//	initializeGrid();
	//	paintExtrustion();
	//}
	//else if (model->mode == model->BSURFACE)
	//{
	//	initializeGrid();
	//	initializeControlPoints();
	//	paintBSurface();
	//}

}

#pragma region Utils
double convertViewportToOpenGLCoordinate(double x)
{
	return (x * 2) - 1;
}

void OpenGlWindow::mousePressEvent(QMouseEvent * event)
{
	if (model->mode == model->BSPLINE){
		Point clickP = Point();
		clickP.x_ = convertViewportToOpenGLCoordinate(event->x() / (double)this->width());
		clickP.y_ = -convertViewportToOpenGLCoordinate(event->y() / (double)this->height());
		clickP.z_ = 0.0f;

		if (model->splineMode == model->CREATEPOINT)
		{
			_points[_currentPolygone].push_back(clickP);
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

void OpenGlWindow::convertPointToFloat(const std::vector<Point>& points, std::vector<float>& pointsF) const
{

	for (int i = 0; i < points.size(); i++)
	{
		pointsF.push_back(points[i].x_);
		pointsF.push_back(points[i].y_);
		pointsF.push_back(points[i].z_);
	}
}

void OpenGlWindow::clear()
{
	_currentPolygone = 0;
	_points.clear();
	repaint();

	_points.push_back(std::vector<Point>());
}

#pragma endregion
