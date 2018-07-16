#include "line.h"

std::ostream &operator<<(std::ostream &os, Line const &line)
{
	line.toStream(os);
	return os;
}
