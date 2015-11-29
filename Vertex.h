#pragma once
#include "Point.h"

class Vertex
{

public:
	
	Point _coords;
	//Edge* _edge;
	int _index;

	Vertex();
	Vertex(const Vertex& v);
	Vertex(const Point coords);
	//Vertex(const Point coords, const Edge edge);
	~Vertex();

	Vertex& operator=(const Vertex& v);
};

