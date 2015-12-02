#pragma once
#include "Edge.h"

class Triangle
{
public:
	Edge _e1;
	Edge _e2;
	Edge _e3;

	int _index1 = 0;
	int _index2 = 0;
	int _index3 = 0;

	Triangle();
	Triangle(const Triangle& triangle);
	Triangle(int index1, int index2, int index3);
	Triangle(const Edge e1, const Edge e2, const Edge e3);
	Triangle(const Edge e1, const Edge e2, const Edge e3, int index1, int index2, int index3);
	~Triangle();

	bool operator == (const Triangle& v) const;
};

