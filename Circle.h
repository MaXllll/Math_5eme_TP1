#pragma once
#include "Point.h"
class Circle
{
public:
	Point _center;
	float _radius;

	Circle();
	Circle(Point c, float r);
	~Circle();

	void CalculateCircle(Point p1, Point p2, Point p3);

};

