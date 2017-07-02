#pragma once

#include <sstream>
#include <iomanip>

#include "line.h"
#include "point.h"


class StrLine :
	public Line
{
public:
	//x * n = b for every point on the line
	Point n = Point(0, 1);
	double b = 0;
	int index = -1;

	StrLine() { type = straight; }

	StrLine(const Point &x, const Point &y) : StrLine()
	{
		n = (y - x).rNormal();
		/*if (n.norm() < pres)
			printf("WTF\n");*/
		b = x * n;
	}

	StrLine(const Point &n_, double b_) : n(n_), b(b_)
	{
		type = straight;
	}

	~StrLine()	{}

	bool isEqual(const StrLine &rhv)
	{
		return true;
	}

	virtual void toStream(std::ostream &os) const
	{
		std::stringstream s;
		s << std::setprecision(presOrd) << std::fixed << std::showpos;
		s << "line   n(" << n.x << ", " << n.y << "), b(" << b << ")";
		os << s.str();
	}

	virtual	bool belongsToMe(const Point& p) const
	{
		return fabs(n * p - b) < pres;
	}

	bool usefullForConstruct(const Point& p, int i)
	{
		if (belongsToMe(p))
			if (index == -1)
			{
				index = i;
				return false;
			}
			else
				return true;
		return false;
	}

	void clearUFC()
	{
		index = -1;
	}
};

