#pragma once

#include <math.h>
#include <exception>
#include <iostream>

#include "UtilsAndConsts.h"

class Point
{
public:
	double x = 0;
	double y = 0;

	Point()	{}

	Point(double x_, double y_) : x(x_), y(y_)
	{}

	~Point() {}

	double norm() const
	{
		return sqrt(x * x + y * y);
	}

	void normalize()
	{
		/*if (*this == zero)
		{
			std::cout <<"trying to normalize a too small point" << std::endl;
			throw new std::exception("trying to normalize a too small point");
		}*/
		double nrm = norm();
		x /= nrm;
		y /= nrm;
	}

	Point rNormal() const
	{
		/*if (*this == zero)
		{
			std::cout << "trying to get normal of a too small point" << std::endl;
			throw new std::exception("trying to get normal of a too small point");
		}*/
		double nrm = norm();
		return Point(-y / nrm, x / nrm);
	}

	

	friend const Point operator+(const Point &lhv, const Point &rhv);
	friend const Point operator-(const Point &lhv, const Point &rhv);
	friend const double operator*(const Point &lhv, const Point &rhv);
	friend const Point operator*(const double &lhv, const Point &rhv);
	friend const Point operator*(const Point &lhv, const double &rhv);
	friend const Point operator/(const Point &lhv, const double &rhv);
	friend const bool operator==(const Point &lhv, const Point &rhv);

	static const Point zero;
};

