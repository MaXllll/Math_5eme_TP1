#pragma once
#include "Edge.h"
class Triangle
{
public:
	Edge _e1;
	Edge _e2;
	Edge _e3;


	Triangle();
	Triangle(const Edge e1, const Edge e2, const Edge e3);
	~Triangle();
};

