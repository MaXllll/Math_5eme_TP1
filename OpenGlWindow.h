#include <QtWidgets\qopenglwidget.h>
#include <QtGui\qevent.h>
#include "Model.h"
#include "Point.h"

class OpenGlWindow : public QOpenGLWidget
{

public:
	OpenGlWindow(Model* model);
	Model* model;
	void newPolygone();
	void clear();

protected:
	void initializeGL();
	void paintGL();
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent *event);


private:
	std::vector<std::vector<Point>> _points = std::vector<std::vector<Point>>();
	int _currentPolygone = -1;
	Point _movingPoint;
	bool _hasClick = false;

	//Points
	void initializePolygone();
	void paintPoints();


	void paintGrid();

	//Utils
	void searchClosedPoint(const Point click, const std::vector<std::vector<Point>>& points, Point& p) const;
	void convertPointToFloat(const std::vector<Point>& points, std::vector<float>& pointsF) const;

};

