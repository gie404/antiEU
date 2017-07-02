#pragma once

#include <sstream>
#include <iomanip>

#include "line.h"
#include "point.h"

class Circle :
	public Line
{
public:
	Point c;
	double r = 1.0;
	int index = -1;
	int c_index = -1;

	Circle() { type = circle; }

	Circle(Point c_, double r_) : Circle() { c = c_; r = r_; }

	Circle(Point c_, Point a) : Circle() { c = c_; r = (a - c).norm(); }

	~Circle()	{}

	virtual void toStream(std::ostream &os) const
	{
		std::stringstream s;
		s << std::setprecision(presOrd) << std::fixed << std::showpos;
		s << "circle c(" << c.x << ", " << c.y << "), r(" << r << ")";
		os << s.str();
	}

	virtual	bool belongsToMe(const Point& p) const
	{
		return fabs((p - c).norm() - r) < pres;
	}

	bool usefullForConstruct(const Point& p, int i)
	{
		if (c_index == -1 && p == c)
		{
			c_index = i;
			return index != -1;
		}
		if (index == -1 && belongsToMe(p))
		{
			index = i;
			return c_index != -1;
		}
		return index != -1 && c_index != -1;
	}

	void clearUFC()
	{
		index = -1;
		c_index = -1;
	}
};

