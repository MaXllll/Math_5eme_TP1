#include "Edge.h"
#include "Vertex.h"
#include "Triangle.h"

Edge::Edge()
{
	_v1 = Vertex();
	_v2 = Vertex();
	//_t1 = new Triangle();
	//_t2 = new Triangle();
}

Edge::Edge(const Edge& e)
{
	Edge();
	_v1 = Vertex(e._v1);
	_v2 = Vertex(e._v2);
	//_t1 = new Triangle(*e._t1);
	//_t2 = new Triangle(*e._t2);
}

Edge::Edge(const Vertex v1, const Vertex v2)
{
	Edge();
	_v1 = v1;
	_v2 = v2;
}

//Edge::Edge(const Vertex v1, const Vertex v2, const Triangle t1, const Triangle t2)
//{
//	*_v1 = v1;
//	*_v2 = v2;
//	*_t1 = Triangle();
//	*_t2 = Triangle();
//}

Edge::~Edge()
{
	//delete _v1;
	//delete _v2;
	//delete _t1;
	//delete _t2;
}

Edge& Edge::operator = (const Edge& v)
{
	_v1 = Vertex(v._v1);
	_v2 = Vertex(v._v2);
	//_t1 = new Triangle(*v._t1);
	//_t2 = new Triangle(*v._t2);
	return *this;
}