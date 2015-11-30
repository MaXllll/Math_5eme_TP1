#include "Circle.h"
#include <math.h>

Circle::Circle()
{

}


Circle::Circle(Point c, float r)
{
	_center = c;
	_radius = r;
}

Circle::~Circle()
{
}

void Circle::CalculateCircle(Point pt1, Point pt2, Point pt3)
{
	double yDelta_a = pt2.y_ - pt1.y_;
	double xDelta_a = pt2.x_ - pt1.x_;
	double yDelta_b = pt3.y_ - pt2.y_;
	double xDelta_b = pt3.x_ - pt2.x_;

	double aSlope = yDelta_a / xDelta_a;
	double bSlope = yDelta_b / xDelta_b;


	this->_center.x_ = (aSlope*bSlope*(pt1.y_ - pt3.y_) + bSlope*(pt1.x_ + pt2.x_)
		- aSlope*(pt2.x_ + pt3.x_)) / (2 * (bSlope - aSlope));
	this->_center.y_ = -1 * (this->_center.x_ - (pt1.x_ + pt2.x_) / 2) / aSlope + (pt1.y_ + pt2.y_) / 2;

	this->_radius = sqrt((pt1.x_ - _center.x_) * (pt1.x_ - _center.x_) + (pt1.y_ - _center.y_) * (pt1.y_ - _center.y_));
}
