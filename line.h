#pragma once

#include <vector>
#include <ostream>

#include "point.h"

enum LineType
{
	straight,
	circle
};

class Line
{
public:
	LineType type;

	Line()
	{
	}

	~Line()
	{
	}

	virtual void toStream(std::ostream& ws) const = 0;
	virtual bool belongsToMe(const Point& p) const = 0;
	/*virtual bool usefullForConstruct(const Point& p, int i) = 0;
	virtual void clearUFC() = 0;*/
};

std::ostream &operator<<(std::ostream &os, Line const &line);
