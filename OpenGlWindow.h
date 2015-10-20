#include <QtWidgets\qopenglwidget.h>
#include <QtGui\qevent.h>
#include "Model.h"
#include "Point.h"

class OpenGlWindow : public QOpenGLWidget
{

public:
	OpenGlWindow(Model* model);
	Model* model;
	void newCluster();
	void clear();
	void GrahamScan();

protected:
	void initializeGL();
	void paintGL();
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent *event);


private:
	std::vector<std::vector<Point>> _points = std::vector<std::vector<Point>>();
	int _currentCluster = -1;
	Point _movingPoint;
	bool _hasClick = false;
	Point _baryCenter;

	//Points
	void initializePolygone();
	void paintPoints(std::vector<float> pointsF) const;
	void PaintBaryCenter() const;


	void paintGrid();

	//Utils
	void searchClosedPoint(const Point click, const std::vector<std::vector<Point>>& points, Point& p) const;
	void convertPointToFloat(const std::vector<Point>& points, std::vector<float>& pointsF) const;
	void ComputeBaryCenter(const std::vector<Point>& points, Point& baryCenter) const;

};

