#pragma once

class Edge;
class Point;
class Vertex
{

public:
	
	Point* _coords;
	Edge* _edge;

	Vertex();
	Vertex(const Point coords, const Edge edge);
	~Vertex();

};

