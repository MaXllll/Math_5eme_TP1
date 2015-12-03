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
	_index1 = triangle._index1;
	_index2 = triangle._index2;
	_index3 = triangle._index3;
}

Triangle::Triangle(const Edge e1, const Edge e2, const Edge e3)
{
	Triangle();
	_e1 = e1;
	_e2 = e2;
	_e3 = e3;
}


Triangle::Triangle(const Edge e1, const Edge e2, const Edge e3, int index1, int index2, int index3)
{
	_e1 = e1;
	_e2 = e2;
	_e3 = e3;
	_index1 = index1;
	_index2 = index2;
	_index3 = index3;
}

Triangle::Triangle(int index1, int index2, int index3)
{
	_index1 = index1;
	_index2 = index2;
	_index3 = index3;
}

Triangle::~Triangle()
{
	//delete _e1;
	//delete _e2;
	//delete _e3;
}

bool Triangle::operator==(const Triangle& t) const
{
	return	_e1 == t._e1 &&
			_e2 == t._e2 &&
			_e3 == t._e3;
}
