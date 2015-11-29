#include <QtWidgets\qopenglwidget.h>
#include <QtGui\qevent.h>
#include "Model.h"
#include "Point.h"
#include "Edge.h"
#include "Triangle.h"
#include "Vertex.h"
#include "CVector.h"
#include <glm\vec3.hpp>

class OpenGlWindow : public QOpenGLWidget
{

public:
	OpenGlWindow(Model* model);
	Model* model;
	void newCluster();
	void clear();
	void GrahamScan();
	void Triangulation();
	void JarvisMarch();	
	void clearCurrentPointAA();

protected:
	void initializeGL();
	void paintGL();
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent *event);


private:
	//Const
	const glm::vec3 pointColor = glm::vec3(1.0f, 0.5f, 0.3f);
	const glm::vec3 pointColor2 = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 baryCenterColor = glm::vec3(0.36f, 0.55f, 0.81f);
	const glm::vec3 lineColor = glm::vec3(1.0f, 0.5f, 0.3f);
		
	std::vector<std::vector<Point>> _points = std::vector<std::vector<Point>>();
	std::vector<std::vector<Point>> _pointsAA = std::vector<std::vector<Point>>();

	int _currentCluster = -1;

	std::vector<Point> vertexGrid = std::vector<Point>();
	std::vector<GLuint> indexGrid = std::vector<GLuint>();
	
	Point _movingPoint;
	bool _hasClick = false;
	Point _baryCenter;


	//Triangulation
	std::vector<Vertex> _vertex = std::vector<Vertex>();
	std::vector<Edge> _edges = std::vector<Edge>();
	std::vector<Triangle> _triangles = std::vector<Triangle>();

	void AddTriangle(Vertex v1, Vertex v2, Point p, int newIndex);
	Point interiorPoint(int currentIndex) const;
	CVector interiorNormal(const Edge& edge, const Point& point) const;


	//Points
	void initializePolygone();
	void paintPoints(std::vector<float>& pointsF) const;
	void PaintBaryCenter() const;
	void paintGrid();

	//Utils
	void searchClosedPoint(const Point click, const std::vector<std::vector<Point>>& points, Point& p) const;
	void convertPointToFloat(const std::vector<Point>& points, std::vector<float>& pointsF, const glm::vec3 color) const;
	void ComputeBaryCenter(const std::vector<Point>& points, Point& baryCenter) const;
	void printVector(const std::vector<Point>& points) const;
	void paintLines(std::vector<float>& pointsF) const;
	void AddBaryCenter(std::vector<float>& pointsF) const;


};

