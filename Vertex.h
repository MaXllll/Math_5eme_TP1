#pragma once
#include "Point.h"
#include "Edge.h"

class Vertex
{

private:
	Point _coords;
	Edge _edge;

public:
	Vertex();
	Vertex(const Point coords, const Edge edge);
	~Vertex();

	//Get
	Point Coords() const;
	Edge EdgeAdj() const;

	//Set
	void Coords(const Point p);
	void EdgeAdj(const Edge e);
};

