#include "Edge.h"


Edge::Edge()
{
	_v1 = Vertex();
	_v2 = Vertex();
	_t1 = Triangle();
	_t2 = Triangle();
}

Edge::Edge(const Vertex v1, const Vertex v2)
{
	_v1 = v1;
	_v2 = v2;
}

Edge::Edge(const Vertex v1, const Vertex v2, const Triangle t1, const Triangle t2)
{
	_v1 = v1;
	_v2 = v2;
	_t1 = Triangle();
	_t2 = Triangle();
}

Edge::~Edge()
{
}
