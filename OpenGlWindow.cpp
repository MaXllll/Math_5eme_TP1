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

#pragma region BSpline

std::vector<GLuint> VAOsP = std::vector<GLuint>();
std::vector<GLuint> VBOsP = std::vector<GLuint>();

std::vector<GLuint> VAOsS = std::vector<GLuint>();
std::vector<GLuint> VBOsS = std::vector<GLuint>();

std::vector<std::vector<Point>> controlPoints = std::vector<std::vector<Point>>();
std::vector<std::vector<float>> bsplineC = std::vector<std::vector<float>>();
std::vector<std::vector<float>> controlP = std::vector<std::vector<float>>();
int currentSpline = -1;

void OpenGlWindow::knot(int n, int c, int x[])
{
	int nplusc, nplus2, i;

	nplusc = n + c;
	nplus2 = n + 2;

	x[1] = 0;
	for (i = 2; i <= nplusc; i++){
		if ((i > c) && (i < nplus2))
			x[i] = x[i - 1] + 1;
		else
			x[i] = x[i - 1];
	}
}

void OpenGlWindow::basis(int c, float t, int npts, int x[], float n[])
{
	int nplusc;
	int i, k;
	float d, e;
	float temp[1000];

	nplusc = npts + c;

	/*		printf("knot vector is \n");
	for (i = 1; i <= nplusc; i++){
	printf(" %d %d \n", i,x[i]);
	}
	printf("t is %f \n", t);
	*/

	/* calculate the first order basis functions n[i][1]	*/

	for (i = 1; i <= nplusc - 1; i++){
		if ((t >= x[i]) && (t < x[i + 1]))
			temp[i] = 1;
		else
			temp[i] = 0;
	}

	/* calculate the higher order basis functions */

	for (k = 2; k <= c; k++){
		for (i = 1; i <= nplusc - k; i++){
			if (temp[i] != 0)    /* if the lower order basis function is zero skip the calculation */
				d = ((t - x[i])*temp[i]) / (x[i + k - 1] - x[i]);
			else
				d = 0;

			if (temp[i + 1] != 0)     /* if the lower order basis function is zero skip the calculation */
				e = ((x[i + k] - t)*temp[i + 1]) / (x[i + k] - x[i + 1]);
			else
				e = 0;

			temp[i] = d + e;
		}
	}

	if (t == (float)x[nplusc]){		/*    pick up last point	*/
		temp[npts] = 1;
	}

	/* put in n array	*/

	for (i = 1; i <= npts; i++) {
		n[i] = temp[i];
	}
}

void OpenGlWindow::bspline(int npts, int k, int p1, int current)
{
	int i, j, icount, jcount;
	int i1;
	int x[1000];		/* allows for 20 data points with basis function of order 5 */
	int nplusc;

	float step;
	float t;
	float nbasis[1000];
	float temp;


	nplusc = npts + k;

	/*  zero and redimension the knot vector and the basis array */

	for (i = 0; i <= npts; i++){
		nbasis[i] = 0.;
	}

	for (i = 0; i <= nplusc; i++){
		x[i] = 0.;
	}

	/* generate the uniform open knot vector */

	knot(npts, k, x);

	/*
	printf("The knot vector is ");
	for (i = 1; i <= nplusc; i++){
	printf(" %d ", x[i]);
	}
	printf("\n");
	*/

	icount = 0;

	/*    calculate the points on the bspline curve */

	t = 0;
	step = ((float)x[nplusc]) / ((float)(p1 - 1));

	for (i1 = 1; i1 <= p1; i1++){

		if ((float)x[nplusc] - t < 5e-6){
			t = (float)x[nplusc];
		}

		basis(k, t, npts, x, nbasis);      /* generate the basis function for this value of t */
		/*
		printf("t = %f \n",t);
		printf("nbasis = ");
		for (i = 1; i <= npts; i++){
		printf("%f  ",nbasis[i]);
		}
		printf("\n");
		*/
		for (j = 1; j <= 3; j++){      /* generate a point on the curve */
			jcount = j;
			float pointC = 0.;

			for (i = 1; i <= npts; i++){ /* Do local matrix multiplication */
				temp = nbasis[i] * controlP[current][jcount];
				pointC = pointC + temp;
				jcount = jcount + 3;
			}
			bsplineC[current].push_back(pointC);
			//bsplineC.push_back(pointC);
		}
		/*
		printf("icount, p %d %f %f %f \n",icount,p[icount+1],p[icount+2],p[icount+3]);
		*/
		icount = icount + 3;
		t = t + step;
	}
}

void OpenGlWindow::calculateBSpline(int i)
{
	bsplineC[i].clear();

	int npts, k, p1;

	npts = controlPoints[i].size();
	k = model->degree;     /* second order, change to 4 to get fourth order */
	p1 = model->pas;   /* eleven points on curve */

	bspline(npts, k, p1, i);
}

void OpenGlWindow::initializeBSpline()
{
	newBSpline();
	movingPoint = new Point();

	for (int i = 0; i < 100; i++)
	{
		VAOsP.push_back(GLuint());
		VBOsP.push_back(GLuint());
		VAOsS.push_back(GLuint());
		VBOsS.push_back(GLuint());
	}
}

void convertPointToFloat(int current){
	controlP[current].clear();
	controlP[current].push_back(0.0);
	for (int i = 0; i < controlPoints[current].size(); i++)
	{
		controlP[current].push_back(controlPoints[current][i].x_);
		controlP[current].push_back(controlPoints[current][i].y_);
		controlP[current].push_back(controlPoints[current][i].z_);
	}
}

void OpenGlWindow::paintBSpline()
{
	bsplineShader.Bind();

	for (int i = 0; i < bsplineC.size(); i++){

		convertPointToFloat(i);

		if (controlP[i].size() > 3){
			calculateBSpline(i);
			controlP[i].erase(controlP[i].begin());
			glGenVertexArrays(i + 1, &VAOsP[i]);
			glGenBuffers(i + 1, &VBOsP[i]);
			glBindVertexArray(VAOsP[i]);

			glBindBuffer(GL_ARRAY_BUFFER, VBOsP[i]);
			glBufferData(GL_ARRAY_BUFFER, controlP[i].size() * sizeof(float), &controlP[i].front(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
		}

		if (bsplineC[i].size() > 0){
			glGenVertexArrays(i + 1, &VAOsS[i]);
			glGenBuffers(i + 1, &VBOsS[i]);
			glBindVertexArray(VAOsS[i]);

			glBindBuffer(GL_ARRAY_BUFFER, VBOsS[i]);
			glBufferData(GL_ARRAY_BUFFER, bsplineC[i].size() * sizeof(float), &bsplineC[i].front(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
		}


		if (controlP[i].size() > 0){
			glBindVertexArray(VAOsP[i]);
			glDrawArrays(GL_POINTS, 0, controlP[i].size() / 3);
			glBindVertexArray(0);
		}

		if (bsplineC[i].size() > 0){
			glBindVertexArray(VAOsS[i]);
			glDrawArrays(GL_LINE_STRIP, 0, bsplineC[i].size() - this->model->pas * 2);
			glBindVertexArray(0);
		}
	}
	bsplineShader.Unbind();
}

void OpenGlWindow::newBSpline()
{
	currentSpline++;
	controlP.push_back(std::vector<float>());
	controlPoints.push_back(std::vector<Point>());
	bsplineC.push_back(std::vector<float>());
}
#pragma endregion

#pragma region BSurface
const GLuint GRID_W = 15, GRID_H = 15;
std::vector<GLfloat> verticesGrid = std::vector<GLfloat>();
std::vector<GLuint> indicesGrid = std::vector<GLuint>();

std::vector<float> BSurfaceControlPoint = std::vector<float>();
std::vector<std::vector<Point>> BSurfaceControlPointP = std::vector<std::vector<Point>>();

std::vector<float> BSurfacePoint = std::vector<float>();
std::vector<GLuint> indexBSurface = std::vector<GLuint>();

Point OpenGlWindow::Decasteljau(float t, const std::vector<Point> &points)
{
	//std::vector<Point> points = polygons[k].get_points();

	std::vector<std::vector<Point>> arbre = std::vector<std::vector<Point>>();
	arbre.push_back(points);
	for (int i = 0; i < points.size() - 1; ++i)
	{
		std::vector<Point> nextLevel = std::vector <Point>();
		for (int j = 0; j < arbre[i].size() - 1; ++j)
		{
			float x = (1.0 - t) * arbre[i][j].x_ + t * arbre[i][j + 1.0].x_;
			float y = (1.0 - t) * arbre[i][j].y_ + t * arbre[i][j + 1.0].y_;
			float z = (1.0 - t) * arbre[i][j].z_ + t * arbre[i][j + 1.0].z_;
			Point a = Point(x, y, z);
			nextLevel.push_back(a);
		}
		arbre.push_back(nextLevel);
	}
	return arbre[points.size() - 1][0];
}

void OpenGlWindow::Decasteljau3D()
{
	BSurfacePoint.clear();
	int pas = 50;
	for (int k = 1; k <= pas; ++k)
	{
		std::vector<Point> middleControlPoints = std::vector<Point>();
		for (int i = 0; i < BSurfaceControlPointP.size(); i++)
		{
			middleControlPoints.push_back(Decasteljau((float)k / (float)pas, BSurfaceControlPointP[i]));
		}

		for (int j = 0; j < pas; j++){
			Point a = Decasteljau((float)j / (float)pas, middleControlPoints);

			BSurfacePoint.push_back(a.x_);
			BSurfacePoint.push_back(a.y_);
			BSurfacePoint.push_back(a.z_);
		}
	}

	//Calculate index BSurface
	int grid_w = pas;
	int grid_h = pas;
	
	for (float i = 0.f; i < grid_h - 1; i += 1.f)
	{
		for (float j = 0.f; j < (grid_w) - 1; j += 1.f)
		{
			float grid_t = grid_w;

			indexBSurface.push_back(i * (grid_t)+j);
			indexBSurface.push_back((i + 1) * (grid_t)+(j + 1));
			indexBSurface.push_back((i + 1) * (grid_t)+j);

			indexBSurface.push_back(i * (grid_t)+j);
			indexBSurface.push_back((i + 1) * (grid_t)+j + 1);
			indexBSurface.push_back(i * (grid_t)+(j + 1));
		}
	}
}	

void OpenGlWindow::initializeControlPoints()
{

	BSurfaceControlPoint.clear();
	BSurfaceControlPointP.clear();
	float incrementW = 0.1f;
	float incrementH = 0.1f;
		
	//float increment = GRID_H >= GRID_W?

	//float incrementW = 3.f / GRID_W;
	//float incrementH = 3.f / GRID_W;


	for (float i = 0.f; i <= 6; i += 1.f)
	{
		BSurfaceControlPointP.push_back(std::vector<Point>());
		float yIncr = 0;
		for (float j = 0.f; j <= 6; j += 1.f)
		{

			BSurfaceControlPoint.push_back(j * incrementW);
			BSurfaceControlPoint.push_back(yIncr);
			BSurfaceControlPoint.push_back((i + 5.f) * incrementH);

			BSurfaceControlPointP[i].push_back(Point(j * incrementW, yIncr, (i + 5.f) * incrementH));

			if (j < 6 / 2)
				yIncr += 0.1f;
			else
				yIncr -= 0.1f;
		}
	}

}

void OpenGlWindow::initializeGrid()
{

	float incrementW = 0.1f;
	float incrementH = 0.1f;

	//float increment = GRID_H >= GRID_W?

	//float incrementW = 3.f / GRID_W;
	//float incrementH = 3.f / GRID_W;


	for (float i = 0.f; i <= GRID_W; i += 1.f)
	{
		float yIncr = 0;
		for (float j = 0.f; j <= GRID_H; j += 1.f)
		{
			verticesGrid.push_back(j * incrementW);
			verticesGrid.push_back(0);
			verticesGrid.push_back(i * incrementH);

			if (j < GRID_H / 2)
				yIncr += 0.1f;
			else
				yIncr -= 0.1f;
		}
	}

	for (float i = 0.f; i < GRID_W; i += 1.f)
	{
		for (float j = 0.f; j < GRID_H; j += 1.f)
		{
			indicesGrid.push_back(i * (GRID_H + 1) + j);
			indicesGrid.push_back((i + 1) * (GRID_H + 1) + (j + 1));
			indicesGrid.push_back((i + 1) * (GRID_H + 1) + j);

			indicesGrid.push_back(i * (GRID_H + 1) + j);
			indicesGrid.push_back((i + 1) * (GRID_H + 1) + j + 1);
			indicesGrid.push_back(i * (GRID_H + 1) + (j + 1));
		}
	}

}

void OpenGlWindow::paintBSurface()
{
	paintGrid();

	/// Control Points
	GLuint VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, BSurfaceControlPoint.size() * sizeof(float), &BSurfaceControlPoint.front(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // Unbind VAO

	basicShader2.Bind();

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	model = glm::rotate(model, /*(GLfloat)glfwGetTime() **/ 1.f, glm::vec3(1.f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3(-0.5f, 0.8f, -2.0f));
	projection = glm::perspective(45.0f, (GLfloat)this->width() / (GLfloat)this->height(), 0.1f, 100.0f);

	GLint modelLoc2 = glGetUniformLocation(basicShader2.GetProgram(), "model");
	GLint viewLoc2 = glGetUniformLocation(basicShader2.GetProgram(), "view");
	GLint projLoc2 = glGetUniformLocation(basicShader2.GetProgram(), "projection");
	GLint isSurfaceLoc2 = glGetUniformLocation(basicShader2.GetProgram(), "isSurface");
	// Pass them to the shaders
	glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
	// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(isSurfaceLoc2, 0);

	glBindVertexArray(VAO2);
	glDrawArrays(GL_POINTS, 0, BSurfaceControlPoint.size());
	glBindVertexArray(0);

	//Bezier Surface

	Decasteljau3D();


	GLuint VBO3, VAO3, EBO3;
	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);

	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, BSurfacePoint.size() * sizeof(float), &BSurfacePoint.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBSurface.size() * sizeof(float), &indexBSurface.front(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glUniform1i(isSurfaceLoc2, 1);

	glBindVertexArray(0); // Unbind VAO

	glBindVertexArray(VAO3);
	glDrawElements(GL_TRIANGLES, indexBSurface.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

void OpenGlWindow::paintGrid()
{

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesGrid.size() * sizeof(float), &verticesGrid.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesGrid.size() * sizeof(float), &indicesGrid.front(), GL_STATIC_DRAW);

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
	glDrawElements(GL_TRIANGLES, indicesGrid.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	basicShader.Unbind();
}
#pragma endregion

#pragma region Extrusion

std::vector<GLfloat> extrusion = std::vector<GLfloat>();
std::vector<GLuint> indexExtrusion = std::vector<GLuint>();

void OpenGlWindow::calculateSimpleExtrusion()
{

	extrusion.clear();
	indexExtrusion.clear();
	if (bsplineC.size() == 0)
		return;
	int height = 50;
	float incr = 0.01;

	for (int i = 0; i < height; i++){
		for (int j = 0; j < bsplineC[0].size(); j += 3)
		{
			extrusion.push_back(bsplineC[0][j] + 0.6f);
			extrusion.push_back(i  * incr);
			extrusion.push_back(bsplineC[0][j + 1] + 0.7f);
		}
	}

	int grid_w = bsplineC[0].size();
	int grid_h = height;

	for (float i = 0.f; i < grid_h - 1; i += 1.f)
	{
		for (float j = 0.f; j < (grid_w / 3) - 1; j += 1.f)
		{
			float grid_t = grid_w / 3;

			indexExtrusion.push_back(i * (grid_t) + j);
			indexExtrusion.push_back((i + 1) * (grid_t) + (j + 1));
			indexExtrusion.push_back((i + 1) * (grid_t) + j);

			indexExtrusion.push_back(i * (grid_t) + j);
			indexExtrusion.push_back((i + 1) * (grid_t) + j + 1);
			indexExtrusion.push_back(i * (grid_t) + (j + 1));
		}
	}
}

void OpenGlWindow::calculateRotationExtrusion()
{
	extrusion.clear();
	indexExtrusion.clear();
	if (bsplineC.size() == 0)
		return;

	double cosX, sinY;

	for (int i = 0; i < 360; i+= 1){

		cosX = cos(i + 1 * PI / 180.0);
		sinY = sin(i + 1 * PI / 180.0);
		//std::cout << result << std::endl;
		for (int j = 0; j < bsplineC[0].size(); j += 3)
		{
			
			extrusion.push_back((bsplineC[0][j]) * sinY);		
			extrusion.push_back(bsplineC[0][j] * cosX);
			extrusion.push_back(bsplineC[0][j + 1]);
			//extrusion.push_back((bsplineC[0][j]));		
			//extrusion.push_back(i * 0.1);
			//extrusion.push_back(bsplineC[0][j + 1]);
		}
	}
	//std::cout << std::endl << std::endl;

	int grid_w = bsplineC[0].size();
	int grid_h = 360;

	for (float i = 0.f; i < grid_h - 1; i += 1.f)
	{
		for (float j = 0.f; j < (grid_w / 3) - 1; j += 1.f)
		{
			float grid_t = grid_w / 3;

			indexExtrusion.push_back(i * (grid_t)+j);
			indexExtrusion.push_back((i + 1) * (grid_t)+(j + 1));
			indexExtrusion.push_back((i + 1) * (grid_t)+j);

			indexExtrusion.push_back(i * (grid_t)+j);
			indexExtrusion.push_back((i + 1) * (grid_t)+j + 1);
			indexExtrusion.push_back(i * (grid_t)+(j + 1));
		}
	}



}

void OpenGlWindow::paintExtrustion()
{
	//paintGrid();

	if (!model->extrusionType)
		calculateSimpleExtrusion();
	else
		calculateRotationExtrusion();

	if (extrusion.size() > 0){
		GLuint VBO2, VAO2, EBO2;
		glGenVertexArrays(1, &VAO2);
		glGenBuffers(1, &VBO2);
		glGenBuffers(1, &EBO2);

		glBindVertexArray(VAO2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, extrusion.size() * sizeof(float), &extrusion.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexExtrusion.size() * sizeof(float), &indexExtrusion.front(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0); // Unbind VAO

		basicShader2.Bind();

		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		model = glm::rotate(model, /*(GLfloat)glfwGetTime() **/ 1.f, glm::vec3(1.f, 0.0f, 0.0f));
		view = glm::translate(view, glm::vec3(-0.5f, 0.8f, -3.0f));
		projection = glm::perspective(45.0f, (GLfloat)this->width() / (GLfloat)this->height(), 0.1f, 100.0f);

		GLint modelLoc2 = glGetUniformLocation(basicShader2.GetProgram(), "model");
		GLint viewLoc2 = glGetUniformLocation(basicShader2.GetProgram(), "view");
		GLint projLoc2 = glGetUniformLocation(basicShader2.GetProgram(), "projection");
		GLint isSurfaceLoc2 = glGetUniformLocation(basicShader2.GetProgram(), "isSurface");
		// Pass them to the shaders
		glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(isSurfaceLoc2, 1);

		glBindVertexArray(VAO2);
		//glDrawArrays(GL_POINTS, 0, extrusion.size());
		glDrawElements(GL_TRIANGLES, indexExtrusion.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

}

#pragma endregion

void OpenGlWindow::initializeGL()
{
	if (model->mode == model->BSPLINE)
	{
		initializeBSpline();
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
		paintBSpline();
	}
	else if (model->mode == model->EXTRUSION)
	{
		initializeGrid();
		paintExtrustion();
	}
	else if (model->mode == model->BSURFACE)
	{
		initializeGrid();
		initializeControlPoints();
		paintBSurface();
	}

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
			controlPoints[currentSpline].push_back(clickP);
			this->update();
		}
		else if (model->splineMode == model->MOVEPOINT)
		{
			hasClick = !hasClick;
			if (hasClick)
			{
				movingPoint = searchClosedPoint(clickP);
			}
		}
		else if (model->splineMode == model->REPEATPOINT)
		{
			if (event->button() == Qt::LeftButton)
			{
				Point* point = searchClosedPoint(clickP);
				if (point != nullptr)
				{
					for (int i = 0; i < controlPoints.size(); i++){
						std::vector<Point>::iterator it = std::find(controlPoints[i].begin(), controlPoints[i].end(), *point);
						if (it != controlPoints[i].end()){
							controlPoints[i].insert(it, *point);
							this->update();
							return;
						}
					}
				}
			}
			else
			{
				Point* point = searchClosedPoint(clickP);
				if (point != nullptr)
				{
					for (int i = 0; i < controlPoints.size(); i++){
						std::vector<Point>::iterator it = std::find(controlPoints[i].begin(), controlPoints[i].end(), *point);
						if (it != controlPoints[i].end()){
							controlPoints[i].erase(it);
							this->update();
							return;
						}
					}
				}
			}
		}
	}
}

void OpenGlWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (model->splineMode == model->MOVEPOINT && hasClick && movingPoint != nullptr)
	{
		movingPoint->x_ = convertViewportToOpenGLCoordinate(event->x() / (double)this->width());
		movingPoint->y_ = -convertViewportToOpenGLCoordinate(event->y() / (double)this->height());
		movingPoint->z_ = 0.0f;
		this->update();
	}
}

Point* OpenGlWindow::searchClosedPoint(Point click)
{
	for (int i = 0; i < controlPoints.size(); i++)
	{
		for (int j = 0; j < controlPoints[i].size(); j++)
		{
			Point* currentPoint = &controlPoints[i][j];
			if (click.isCloseTo(*currentPoint, 0.05)){
				return currentPoint;
			}
		}
	}

	return nullptr;
}

void OpenGlWindow::clear()
{
	currentSpline = 0;
	controlPoints.clear();
	bsplineC.clear();
	controlP.clear();
	repaint();

	controlP.push_back(std::vector<float>());
	controlPoints.push_back(std::vector<Point>());
	bsplineC.push_back(std::vector<float>());
}

#pragma endregion
