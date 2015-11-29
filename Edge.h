#pragma once
#include "Vertex.h"

class Edge
{

public:
	Vertex _v1;
	Vertex _v2;

	//Triangle* _t1;
	//Triangle* _t2;

	Edge();
	Edge(const Edge& e);
	Edge(const Vertex v1, const Vertex v2);
	//Edge(const Vertex v1, const Vertex v2, const Triangle t1, const Triangle t2);
	~Edge();


	bool operator==(const Edge& e) const;

	//bool operator!=(const Edge& e)
	//{
	//	return !(e == (*this));
	//}
};

