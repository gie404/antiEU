#include "CompareLine.h"

bool operator==(const Line &lhv, const Line &rhv)
{
	if (lhv.type == straight && rhv.type == straight)
		return *static_cast<const StrLine*>(&lhv) == *static_cast<const StrLine*>(&rhv);

	if (lhv.type == straight && rhv.type == circle)
		return false;

	if (lhv.type == circle && rhv.type == straight)
		return false;

	if (lhv.type == circle && rhv.type == circle)
		return *static_cast<const Circle*>(&lhv) == *static_cast<const Circle*>(&rhv);

	return false;
}

bool operator==(const StrLine &lhv, const StrLine &rhv)
{
	if(fabs(lhv.b) > pres)
		return lhv.b * lhv.n == rhv.b * rhv.n;
	else
		if (fabs(rhv.b) > pres)
			return false;
	
	return lhv.n == rhv.n || lhv.n + rhv.n == Point::zero;
}

bool operator==(const Circle &lhv, const Circle &rhv)
{
	return lhv.c == rhv.c && fabs(lhv.r - rhv.r) < pres;
}
