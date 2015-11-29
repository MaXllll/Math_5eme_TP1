#include "Triangle.h"
#include "Edge.h"

Triangle::Triangle()
{
	_e1 = Edge();
	_e2 = Edge();
	_e3 = Edge();
}

Triangle::Triangle(const Triangle& triangle)
{
	Triangle();
	_e1 = Edge(triangle._e1);
	_e2 = Edge(triangle._e2);
	_e3 = Edge(triangle._e3);
}

Triangle::Triangle(const Edge e1, const Edge e2, const Edge e3)
{
	Triangle();
	_e1 = e1;
	_e2 = e2;
	_e3 = e3;
}

Triangle::~Triangle()
{
	//delete _e1;
	//delete _e2;
	//delete _e3;
}

Triangle& Triangle::operator = (const Triangle& t)
{
	_e1 = Edge(t._e1);
	_e2 = Edge(t._e2);
	_e3 = Edge(t._e3);
	return *this;
}
