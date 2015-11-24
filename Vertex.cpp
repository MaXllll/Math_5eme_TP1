#include "Vertex.h"


Vertex::Vertex()
{
	_coords = Point();
	_edge = Edge();
}

Vertex::Vertex(const Point coords, const Edge edge)
{
	_coords = coords;
	_edge = edge;
}

Point Vertex::Coords() const
{
	return _coords;
}

Edge Vertex::EdgeAdj() const
{
	return _edge;
}

void  Vertex::Coords(const Point p)
{
	_coords = p;
}

void Vertex::EdgeAdj(const Edge e)
{
	_edge = e;
}


Vertex::~Vertex()
{
}
