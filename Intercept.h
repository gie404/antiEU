#pragma once

#include <vector>
#include <math.h>

#include "UtilsAndConsts.h"
#include "StrLine.h"
#include "Circle.h"

class Intercept
{
public:
	static unsigned intercept(const Line &lhv, const Line &rhv, Point &x, Point &y)
	{
		if(lhv.type == straight && rhv.type == straight)
			return intercept(*static_cast<const StrLine*>(&lhv), *static_cast<const StrLine*>(&rhv), x);

		if (lhv.type == straight && rhv.type == circle)
			return intercept(*static_cast<const StrLine*>(&lhv), *static_cast<const Circle*>(&rhv), x, y);

		if (lhv.type == circle && rhv.type == straight)
			return intercept(*static_cast<const Circle*>(&lhv), *static_cast<const StrLine*>(&rhv), x, y);

		if (lhv.type == circle && rhv.type == circle)
			return intercept(*static_cast<const Circle*>(&lhv), *static_cast<const Circle*>(&rhv), x, y);

		return 0;
	}

	static unsigned intercept(const StrLine &lhv, const Circle &rhv, Point &x, Point &y)
	{ return interLC(lhv, rhv, x, y);	}
	
	static unsigned intercept(const Circle &lhv, const StrLine &rhv, Point &x, Point &y)
	{ return interLC(rhv, lhv, x, y);	}

	static unsigned intercept(const StrLine &line1, const StrLine &line2, Point &x)
	{ return interLL(line1, line2, x);	}

	static unsigned intercept(const Circle &circle1, const Circle &circle2, Point &x, Point &y)
	{ return interCC(circle1, circle2, x, y); }


	static unsigned interLC(const StrLine &line, const Circle &circle, Point &x, Point &y)
	{
		double shift = line.b - circle.c * line.n;
		double disc = circle.r * circle.r - shift * shift;

		if (disc < 0)
			return 0;
		
		x = circle.c + shift * line.n;
		if (fabs(disc) < pres)
		{
			return 1;
		}

		Point cat = sqrt(disc) * line.n.rNormal();
		y = x + cat;
		x = x - cat;
		return 2;
	}

	static unsigned interLL(const StrLine &line1, const StrLine &line2, Point &x)
	{
		Point t1(line1.n.y, -line1.n.x);

		double disc = t1 * line2.n;

		if (fabs(disc) < pres)
		{
			return 0;
		}

		x = line1.b * line1.n + ((line2.b - line1.b * (line1.n * line2.n)) / disc) * t1;
		return 1;
	}

	static unsigned interCC(const Circle &circle1, const Circle &circle2, Point &x, Point &y)
	{
		Point shift = circle2.c - circle1.c;
		double m = shift.norm();
		if (fabs(m) < pres)
			return 0;

		Point t = shift / m;
		Point n = t.rNormal();
		double r1 = circle1.r;
		double r2 = circle2.r;
		double a = (r1 * r1 - r2 * r2 + m * m) / (2 * m);
		double disc = r1 * r1 - a * a;
		if (disc < 0)
			return 0;
		
		x = circle1.c + a * t;
		if (fabs(disc) < pres)
		{
			return 1;
		}

		Point cat = sqrt(disc) * t.rNormal();
		y = x + cat;
		x = x - cat;
		return 2;
	}
};

