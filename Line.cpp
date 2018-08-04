#include "line.h"

std::ostream &operator<<(std::ostream& os, const Line& line)
{
	line.toStream(os);
	return os;
}
