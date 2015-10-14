#include <QtWidgets\qopenglwidget.h>
#include <QtGui\qevent.h>
#include "Model.h"
#include "Point.h"

class OpenGlWindow : public QOpenGLWidget
{

public:
	OpenGlWindow(Model* model);
	Model* model;
	void newBSpline();
	void clear();

protected:
	void initializeGL();
	void paintGL();
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent *event);


private:
	//BSpline
	void knot(int n, int c, int x[]);
	void basis(int c, float t, int npts, int x[], float n[]);
	void bspline(int npts, int k, int p1, int current);
	void calculateBSpline(int i);
	void initializeBSpline();
	void paintBSpline();

	//B Surface
	Point Decasteljau(float t, const std::vector<Point> &points);
	void Decasteljau3D();
	void CalculateBezier();
	void initializeGrid();
	void initializeControlPoints();
	void paintBSurface();

	void paintGrid();

	//Extrusion
	void calculateSimpleExtrusion();
	void calculateRotationExtrusion();
	void paintExtrustion();

	//Utils
	Point* searchClosedPoint(Point click);

	Point* movingPoint;
	bool hasClick = false;

};

