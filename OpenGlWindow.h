#include <QtWidgets\qopenglwidget.h>
#include <QtGui\qevent.h>
#include "Model.h"
#include "Point.h"
#include "Edge.h"
#include "Triangle.h"
#include "Vertex.h"
#include "CVector.h"
#include <glm\vec3.hpp>
#include "CVector.h"

class OpenGlWindow : public QOpenGLWidget
{

public:
	OpenGlWindow(Model* model);
	Model* model;
	void newCluster();
	void clear();
	void GrahamScan();
	void Triangulation(bool flipping);
	void JarvisMarch();	
	void clearCurrentPointAA();
	void voronoi();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent *event);


private:
	//Const
	const glm::vec3 pointColor = glm::vec3(1.0f, 0.5f, 0.3f);
	const glm::vec3 pointColor2 = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 baryCenterColor = glm::vec3(0.36f, 0.55f, 0.81f);
	const glm::vec3 lineColor = glm::vec3(1.0f, 0.5f, 0.3f);
	const glm::vec3 voronoiColor = glm::vec3(1.0f, 1.0f, 1.0f);
		
	std::vector<std::vector<Point>> _points = std::vector<std::vector<Point>>();
	std::vector<std::vector<Point>> _pointsAA = std::vector<std::vector<Point>>();

	int _currentCluster = -1;

	std::vector<Point> vertexGrid = std::vector<Point>();
	std::vector<GLuint> indexGrid = std::vector<GLuint>();
	
	Point _movingPoint;
	Point* _movingPointPtr = nullptr; 
	bool _hasClick = false;
	Point _baryCenter;


	//Triangulation
	std::vector<Vertex> _vertex = std::vector<Vertex>();
	std::vector<Edge> _edges = std::vector<Edge>();
	std::vector<Triangle> _triangles = std::vector<Triangle>();

	std::vector<Triangle> _trianglesIndex = std::vector<Triangle>();

	//std::vector<Edge> _edgesExt = std::vector<Edge>();

	std::map<Edge, std::vector<Triangle>> _edgeToTriangle = std::map<Edge, std::vector<Triangle>>();

	bool isDelaunay(Triangle t1, Triangle t2) const;
	void CalculateCircle(Point p1, Point p2, Point p3);
	void AddTriangleNew(Vertex v1, Vertex v2, Point p, int newIndex);
	Point interiorPoint(Edge currentEdge) const;
	CVector interiorNormal(const Edge& edge, const Point& point) const;
	void findTrianglePoints(const Triangle& t1, const Triangle& t2, std::vector<Vertex>& outVec) const;
	void AddTriangle(Vertex v1, Vertex v2, Vertex v3);
	void feedIndexGrid();

	int pointIndex = 0;

	//Voronoi
	std::vector<Point> _centers = std::vector<Point>();
	std::vector<int> _indexCenters = std::vector<int>();

	//Points
	void initializePolygone();
	void paintPoints(std::vector<float>& pointsF) const;
	void PaintBaryCenter() const;
	void paintGrid();
	void paintVoronoi();


	//Utils
	void searchClosedPoint(const Point click, std::vector<std::vector<Point>>& points);
	void convertPointToFloat(const std::vector<Point>& points, std::vector<float>& pointsF, const glm::vec3 color) const;
	void ComputeBaryCenter(const std::vector<Point>& points, Point& baryCenter) const;
	void printVector(const std::vector<Point>& points) const;
	void paintLines(std::vector<float>& pointsF) const;
	void AddBaryCenter(std::vector<float>& pointsF) const;
	bool isConvex(CVector& v, CVector& v2) const;
	bool intersection(const Point& a, const Point& b, const Point& c, const Point& d, Point& intersec) const;
	float determinant(float matrix[2][2]) const;
};

