#include "Vertex.h"
#include "Point.h"
#include "Edge.h"

Vertex::Vertex()
{
	_coords = new Point();
	_edge = new Edge();
}

Vertex::Vertex(const Point coords, const Edge edge)
{
	*_coords = coords;
	*_edge = edge;
}


Vertex::~Vertex()
{
}
