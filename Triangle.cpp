#include "Triangle.h"
#include "Edge.h"

Triangle::Triangle()
{

}


Triangle::Triangle(const Edge e1, const Edge e2, const Edge e3)
{
	*_e1 = e1;
	*_e2 = e2;
	*_e3 = e3;
}

Triangle::~Triangle()
{

}
