//
//  Segment.h
//  Projet Math
//
//  Created by Maxime Lahaye and Mathieu Harharidis.
//

#ifndef __Cours1__Segment__
#define __Cours1__Segment__

#include <stdio.h>
#include "Point.h"

class CVector
{
public:
	float x;
	float y;
    
private:
    Point p1;
    Point p2;

public:
    /* Constructor and Destructor*/
    CVector();
    CVector(const Point &p1,const Point &p2);
	CVector(float x, float y);
    CVector(const CVector &c);
    ~CVector(void);
    
    /* Getter and Setter */
    Point get_p1() const;
    Point get_p2() const;
    void set_p1(const Point p1);
    void set_p2(const Point p2);
    
    float diff_x() const;
    float diff_y() const;
    
    CVector normal() const;
	float norm() const;
	float dotProduct(CVector vec) const;
	float dotProductMag(CVector vec) const;
	float angle(CVector vec) const;
	float crossProduct(CVector v2) const;
};

std::ostream& operator<<(std::ostream& out, const CVector &c);

#endif /* defined(__Cours1__Segment__) */
