#include "Vertex.h"
#include "Point.h"
#include "Edge.h"

Vertex::Vertex()
{
	_coords = Point();
	//_edge = new Edge();
}

Vertex::Vertex(const Vertex& v)
{
	Vertex();
	//_edge = new Edge(*v._edge);
	_coords = Point(v._coords);
	_index = v._index;
}


Vertex::Vertex(const Point coords)
{
	Vertex();
	_coords = coords;
}


//Vertex::Vertex(const Point coords, const Edge edge)
//{
//	Vertex();
//	*_coords = coords;
//	*_edge = edge;
//}


Vertex::~Vertex()
{
	//delete _coords;
	//delete _edge;
}

Vertex& Vertex::operator = (const Vertex& v)
{
	//_edge = new Edge(*v._edge);
	_coords = Point(v._coords);
	return *this;
}
