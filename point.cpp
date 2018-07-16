#include "point.h"

const Point Point::zero = Point(0, 0);

const Point operator+(const Point &lhv, const Point &rhv)
{
	return Point(lhv.x + rhv.x, lhv.y + rhv.y);
}

const Point operator-(const Point &lhv, const Point &rhv)
{
	return Point(lhv.x - rhv.x, lhv.y - rhv.y);
}

const double operator*(const Point &lhv, const Point &rhv)
{
	return lhv.x * rhv.x + lhv.y * rhv.y;
}

const Point operator*(const double &lhv, const Point &rhv)
{
	return Point(rhv.x * lhv, rhv.y * lhv);
}

const Point operator*(const Point &lhv, const double &rhv)
{
	return Point(lhv.x * rhv, lhv.y * rhv);
}

const Point operator/(const Point &lhv, const double &rhv)
{
	return Point(lhv.x / rhv, lhv.y / rhv);
}

const bool operator==(const Point &lhv, const Point &rhv)
{
	/*long lx = (long)round(lhv.x * invPres);
	long ly = (long)round(lhv.y * invPres);
	long rx = (long)round(rhv.x * invPres);
	long ry = (long)round(rhv.y * invPres);*/
	/*long xx = (long)round((lhv.x - rhv.x) * invPres);
	long xx = (long)round((lhv.x - rhv.y) * invPres);*/

	return (fabs(lhv.x - rhv.x) < pres) && (fabs(lhv.y - rhv.y) < pres);
	//return ((long)round((lhv.x - rhv.x) * invPres)) == 0 && ((long)round((lhv.y - rhv.y) * invPres)) == 0;
	//return ((long)round(lhv.x * invPres)) == ((long)round(rhv.x * invPres)) && ((long)round(lhv.y * invPres)) == ((long)round(rhv.y * invPres));
}
