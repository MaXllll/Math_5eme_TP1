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

void Circle::CalculateCircle(Point p0, Point p1, Point p2)
{
	float dA = p0.x_ * p0.x_ + p0.y_ * p0.y_;
	float dB = p1.x_ * p1.x_ + p1.y_ * p1.y_;
	float dC = p2.x_ * p2.x_ + p2.y_ * p2.y_;

	float aux1 = (dA*(p2.y_ - p1.y_) + dB*(p0.y_ - p2.y_) + dC*(p1.y_ - p0.y_));
	float aux2 = -(dA*(p2.x_ - p1.x_) + dB*(p0.x_ - p2.x_) + dC*(p1.x_ - p0.x_));
	float div = (2 * (p0.x_*(p2.y_ - p1.y_) + p1.x_*(p0.y_ - p2.y_) + p2.x_*(p1.y_ - p0.y_)));

	if (div == 0){
		return;
	}

	_center.x_ = aux1 / div;
	_center.y_ = aux2 / div;

	this->_radius = sqrt((p0.x_ - _center.x_) * (p0.x_ - _center.x_) + (p0.y_ - _center.y_) * (p0.y_ - _center.y_));
}
