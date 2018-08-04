
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <iostream>
#include <cstring>

#include "point.h"
#include "StrLine.h"
#include "Circle.h"
#include "Intercept.h"
#include "CompareLine.h"

#include "simple_svg_1.0.0.hpp"

uint64_t fcalls = 0;


struct RefPoint
{
	Point p;
	uint32_t inh;

	RefPoint() {}
	RefPoint(const Point &p_, uint32_t inh_) : p(p_), inh(inh_) {}
};

struct GoalRefPoint : RefPoint
{
	bool onLine = false;

	GoalRefPoint() {}
	GoalRefPoint(const Point &p_, uint32_t inh_, bool onLine_) : RefPoint(p_, inh_), onLine(onLine_) {}
};

struct RefLine
{
	Line *l;
	uint32_t inh;

	RefLine() {}
	
	//RefLine(const Line &l_, uint32_t inh_) : l(&l_), inh(inh_) {}
	
	RefLine(Line* l_, uint32_t inh_) : l(l_), inh(inh_) {}
	
	//~RefLine() { printf("dtor refline\n"); }
};

void create_svg(const Line &newLine, std::vector<RefLine> &lines)
{
	svg::Dimensions dimensions(1000, 1000);
	svg::Document doc("my_svg.svg", svg::Layout(dimensions, svg::Layout::BottomLeft));

	// Red image border.
	svg::Polygon border(svg::Stroke(1, svg::Color::Red));
	border << svg::Point(0, 0) << svg::Point(dimensions.width, 0)
		<< svg::Point(dimensions.width, dimensions.height) << svg::Point(0, dimensions.height);
	doc << border;

	double sc = 50;
	double sx = 500 - 2.5 * sc, sy = 500 - 2.5 * sc;
	
	svg::Color::Defaults clr(svg::Color::Defaults::Aqua);
	
	std::vector<RefLine> total(lines);
	total.push_back(RefLine(const_cast<Line*>(&newLine), 0));
	
	for (const RefLine rline : total)
	{
		if (rline.l->type == straight)
		{
			const StrLine* l = static_cast<const StrLine*>(rline.l);
			if (fabs(l->n.y) < 1e-7)
				doc << svg::Line(svg::Point((l->b + 50 * l->n.y) / l->n.x * sc + sx, -50 * sc + sy), svg::Point((l->b - 50 * l->n.y) / l->n.x * sc + sx, +50 * sc + sy), svg::Stroke(0.5, clr));
			else
				doc << svg::Line(svg::Point(-50 * sc + sx, (l->b + 50 * l->n.x) / l->n.y * sc + sy), svg::Point(+50 * sc + sx, (l->b - 50 * l->n.x) / l->n.y * sc + sy), svg::Stroke(0.5, clr));
		}
		else
		{
			const Circle* l = static_cast<const Circle*>(rline.l);
			doc << svg::Circle(svg::Point(l->c.x * sc + sx, l->c.y * sc + sy), 2 * l->r * sc, svg::Fill(svg::Color::Transparent), svg::Stroke(0.5, clr));
		}
		clr = static_cast<svg::Color::Defaults>( (static_cast<uint32_t>(clr) + 1) % static_cast<uint32_t>(svg::Color::Defaults::Yellow) );
	}

	/*Point A(0, 0);
	Point B(1.6, 2);
	Point C(3, 2);
	Point D(4, 0);

	doc << svg::Circle(svg::Point(0 + sx, 0 + sy), 10, svg::Fill(svg::Color(0, 0, 255)), svg::Stroke(1, svg::Color(200, 250, 150)));
	doc << svg::Circle(svg::Point(1.6 * sc + sx, 2 * sc + sy), 10, svg::Fill(svg::Color(0, 0, 255)), svg::Stroke(1, svg::Color(200, 250, 150)));
	doc << svg::Circle(svg::Point(3 * sc + sx, 2 * sc + sy), 10, svg::Fill(svg::Color(0, 0, 255)), svg::Stroke(1, svg::Color(200, 250, 150)));
	doc << svg::Circle(svg::Point(4 * sc + sx, 0 * sc + sy), 10, svg::Fill(svg::Color(0, 0, 255)), svg::Stroke(1, svg::Color(200, 250, 150)));*/

	// Condensed notation, parenthesis isolate temporaries that are inserted into parents.
	/*doc << (svg::LineChart(svg::Dimensions(65, 5))
		<< (svg::Polyline(svg::Stroke(.5, svg::Color::Blue)) << svg::Point(0, 0) << svg::Point(10, 8) << svg::Point(20, 13))
		<< (svg::Polyline(svg::Stroke(.5, svg::Color::Orange)) << svg::Point(0, 10) << svg::Point(10, 16) << svg::Point(20, 20))
		<< (svg::Polyline(svg::Stroke(.5, svg::Color::Cyan)) << svg::Point(0, 5) << svg::Point(10, 13) << svg::Point(20, 16)));
*/
	/*doc << svg::Text(svg::Point(5, 77), "Simple SVG", svg::Color::Silver, svg::Font(10, "Verdana"));

	doc << (svg::Polygon(svg::Color(200, 160, 220), svg::Stroke(.5, svg::Color(150, 160, 200))) << svg::Point(20, 70)
		<< svg::Point(25, 72) << svg::Point(33, 70) << svg::Point(35, 60) << svg::Point(25, 55) << svg::Point(18, 63));

	doc << svg::Rectangle(svg::Point(70, 55), 20, 15, svg::Color::Yellow);

	doc << svg::Line(svg::Point(-700, 25), svg::Point(700, 25), svg::Stroke(1.5, svg::Color(0, 0, 255)));

	doc << svg::Circle(svg::Point(60, 10), 10, svg::Fill(svg::Color(0, 0, 255)), svg::Stroke(1, svg::Color(200, 250, 150)));*/
	std::cout << "saving doc result " << doc.save();
}

inline bool checkForPoint(std::vector<RefPoint> &points, Point& p) noexcept
{
	for (const RefPoint& rp : points)
	{
		if (rp.p == p)
			return true;
	}
	return false;
}

void printMatch(const Line &newLine, std::vector<RefLine> &lines, unsigned level)
{
	static bool need_svg = true;
	if (need_svg)
	{
		create_svg(newLine, lines);
		need_svg = false;
	}

	printf("Match, level %d, lines %ld -----------------------\n", level, lines.size() + 1);
	for (const RefLine rline : lines)
	{
		std::cout << *(rline.l) << std::endl;
	}
	std::cout << newLine << std::endl;
	return;
}

inline bool checkForLine(std::vector<RefLine> &lines, Line &newLine) noexcept
{
	for (const RefLine& rLine : lines)
	{
		if (*rLine.l == newLine)
			return true;
	}
	return false;
}

void findLine(Line &&newLine, uint32_t newLineInh, std::vector<RefPoint> &refPoints, std::vector<RefLine> &lines, std::vector<RefLine> &Lgoals,
	std::vector<GoalRefPoint*> &Pgoals, int** RP_Table, unsigned level, unsigned maxLevel, bool isNewLine)
//void findLine(const Line &&newLine, unsigned level, bool isNewLine)
{
	size_t baseSize = refPoints.size();
	RefLine *Lgoal = nullptr;
	std::vector<GoalRefPoint*> matchedPgoals;
	
	fcalls++;

	if (isNewLine)
	{
		for (const RefLine& rLine : lines)
		{
			if (*rLine.l == newLine)
				return;
		}
		
		for  (GoalRefPoint* point : Pgoals)
		{
			if (point->onLine == false)
			{
				if(newLine.belongsToMe(point->p))
					point->onLine = true;
				else
					if(level + 1 == maxLevel)
						return;
			}
		}
		

		auto LgoaltoDel = Lgoals.end();
		for (auto it = Lgoals.begin(); it != Lgoals.end(); ++it)
		{
			if (newLine == *it->l)
			{
				if (Lgoals.size() == 1 && Pgoals.size() == 0)
				{
					printMatch(newLine, lines, level);
				}
				else
				{
					LgoaltoDel = it;
					break;
				}
			}
		}

		if (level >= maxLevel && Pgoals.size() == 0)
			return;

		
		for (const RefLine& rline : lines)
		{
			RefPoint a, b;
			unsigned num = Intercept::intercept(*rline.l, newLine, a.p, b.p);

			bool newA = num > 0;
			bool newB = num > 1;
			for (const RefPoint &rp : refPoints)
			{
				if (!newA && !newB)
					break;
				if (newA && rp.p == a.p)
					newA = false;
				if (newB && rp.p == b.p)
					newB = false;
			}
			
			auto PgoaltoDelA = Pgoals.end();
			auto PgoaltoDelB = Pgoals.end();
			if (newA)
			{
				for (auto it = Pgoals.begin(); it != Pgoals.end(); ++it)
				{
					if (a.p == (**it).p)
					{
						PgoaltoDelA = it;
						break;
					}
				}
				if (PgoaltoDelA != Pgoals.end())
				{
					matchedPgoals.push_back(*PgoaltoDelA);
					Pgoals.erase(PgoaltoDelA);
				}
				a.inh = rline.inh | newLineInh;
				refPoints.push_back(a);
			}
			if (newB)
			{
				PgoaltoDelB = Pgoals.end();//previos Pgoals.end invalidated after erasing PgoaltoDelA
				for (auto it = Pgoals.begin(); it != Pgoals.end(); ++it)
				{
					if (b.p == (**it).p)
					{
						PgoaltoDelB = it;
						break;
					}
				}
				if (PgoaltoDelB != Pgoals.end())
				{
					matchedPgoals.push_back(*PgoaltoDelB);
					Pgoals.erase(PgoaltoDelB);
				}
				b.inh = rline.inh | newLineInh;
				refPoints.push_back(b);
			}
		}

		if (LgoaltoDel != Lgoals.end())
		{
			Lgoal = &(*LgoaltoDel);
			Lgoals.erase(LgoaltoDel);
		}

		if (Lgoals.size() == 0 && Pgoals.size() == 0)
		{
			printMatch(newLine, lines, level);
			refPoints.resize(baseSize);
			if (Lgoal != nullptr)
				Lgoals.push_back(*Lgoal);
			Pgoals.insert(Pgoals.end(), matchedPgoals.begin(), matchedPgoals.end());
			return;
		}
		//printf("before resize %d, %lld, base %lld, level %d\n", refPoints.size(), lines.size(), baseSize, level);
		if (level >= maxLevel)
		{
			refPoints.resize(baseSize);
			if (Lgoal != nullptr)
				Lgoals.push_back(*Lgoal);
			Pgoals.insert(Pgoals.end(), matchedPgoals.begin(), matchedPgoals.end());
			return;
		}

		/*size_t maxSize = refPoints.size();
		for (size_t i = 0; i < baseSize; ++i)
			memset(RP_Table[i] + baseSize, 0, (maxSize - baseSize) * sizeof(int));

		for (size_t i = baseSize; i < maxSize; ++i)
			memset(RP_Table[i], 0, maxSize * sizeof(int));*/


		lines.push_back(RefLine(&newLine, newLineInh));
	}

	if (level + Lgoals.size() <= maxLevel)
	{
		if (level + Lgoals.size() == maxLevel)
		{
			for (int k = 0; k < Lgoals.size(); ++k)
			{
				Line* line = Lgoals[k].l;
				if (line->type == LineType::straight)
				{
					StrLine* sl = static_cast<StrLine*>(line);
					sl->clearUFC();
					for (int j = 0; j < refPoints.size(); ++j)
					{
						if (sl->usefullForConstruct(refPoints[j].p, j))
						{
							int i = sl->index;
							uint32_t inh = refPoints[i].inh | refPoints[j].inh;
							RP_Table[i][j] |= (1 << 0);
							findLine(StrLine(refPoints[i].p, refPoints[j].p), inh, refPoints, lines, Lgoals, Pgoals, RP_Table, level + 1, maxLevel, true);
							RP_Table[i][j] &= ~(1 << 0);
							break;
						}
					}
				}
				if (line->type == LineType::circle)
				{
					Circle* cl = static_cast<Circle*>(line);
					cl->clearUFC();
					for (int j = 0; j < refPoints.size(); ++j)
					{
						if (cl->usefullForConstruct(refPoints[j].p, j))
						{
							int j = cl->index;
							int i = cl->c_index;
							uint32_t inh = refPoints[i].inh | refPoints[j].inh;
							RP_Table[i][j] |= (1 << 1);
							findLine(Circle(refPoints[i].p, refPoints[j].p), inh, refPoints, lines, Lgoals, Pgoals, RP_Table, level + 1, maxLevel, true);
							RP_Table[i][j] &= ~(1 << 1);
							break;
						}
					}
				}
			}
		}
		else
		{
			bool needForCheckInh = Lgoals.size() == 1 && (level + 1 == maxLevel);

			//for (auto it = refPoints.begin(); it != refPoints.end(); ++it)
			for (int i = 0; i < refPoints.size();++i)
			{
				//for (auto jt = refPoints.begin(); jt != refPoints.end(); ++jt)
				for (int j = 0; j < refPoints.size();++j)
				{

					if (i == j)
						continue;

					/*if ((refPoints[i].p == refPoints[j].p))
						printf("wtf\n");*/

					uint32_t inh = refPoints[i].inh | refPoints[j].inh;
					if (needForCheckInh && (Lgoals[0].inh > (Lgoals[0].inh & inh)))
						continue;//not enough inh to be in Lgoals


					if (i < j && ((RP_Table[i][j] & (1 << 0)) == 0))
					{
						RP_Table[i][j] |= (1 << 0);
						findLine(StrLine(refPoints[i].p, refPoints[j].p), inh, refPoints, lines, Lgoals, Pgoals, RP_Table, level + 1, maxLevel, true);
						RP_Table[i][j] &= ~(1 << 0);
					}
					if ((RP_Table[i][j] & (1 << 1)) == 0)
					{
						RP_Table[i][j] |= (1 << 1);
						findLine(Circle(refPoints[i].p, refPoints[j].p), inh, refPoints, lines, Lgoals, Pgoals, RP_Table, level + 1, maxLevel, true);
						RP_Table[i][j] &= ~(1 << 1);
					}
				}
			}
		}
	}

	if (isNewLine)
	{
		/*printf("before resize %d, %lld, base %lld, level %d\n", refPoints.size(), lines.size(), baseSize, level);*/
		refPoints.resize(baseSize);
		//printf("after resize  %d, %d, base %d, level %d\n", refPoints.size(), lines.size(), baseSize, level);
		lines.resize(lines.size() - 1);

		if (Lgoal != nullptr)
			Lgoals.push_back(*Lgoal);

		Pgoals.insert(Pgoals.end(), matchedPgoals.begin(), matchedPgoals.end());
	}

}

long N(int n, int n0)
{
	return (n * n - n) * 2 / 3 + n0;
}

int antiEu(long int depth)
{
	using namespace std;
	using namespace std::chrono;
	
	steady_clock::time_point t1 = steady_clock::now();

	vector<RefPoint> refPoints;
	vector<RefLine> lines;
	vector<RefLine> Lgoals;
	vector<GoalRefPoint*> Pgoals;
	const auto maxLevel = static_cast<unsigned>(depth);

	refPoints.reserve(10000);
	lines.reserve(10000);
	
	Point A(0, 0);
	Point B(4.31, 0);
	Point C(6, 0);
	Point D = (A + B) / 2;
	Point E = (C + B) / 2;
	Point F, G, H = C / 3;
	
	//~ StrLine rab(A, B);
	//~ StrLine rac(A, C);
	//~ StrLine rbc(B, C);
	float r1 = 1.1;
	float r2 = 2.7;
	Circle c1(A, r1);
	Circle c2(B, r2);
	Circle c3(A, 2*r2 - 2*r1);
	Circle c4(B, 2*r2 - 2*r1);
	Circle c5(A, B);
	Circle c6(B, A);
	
	refPoints.push_back(RefPoint(A, 1 << 0));
	refPoints.push_back(RefPoint(B, 1 << 1));
	//~ refPoints.push_back(RefPoint(C, 1 << 2));
	
	lines.push_back(RefLine(&c1, 1 << 0));
	lines.push_back(RefLine(&c2, 1 << 1));
	//~ lines.push_back(RefLine(&rbc, 1 << 1 & 1 << 2));

	//~ lines.push_back(RefLine(&c1, 7));
	Intercept::intercept(c3, c6, C, D);
	StrLine tmp(C, A);
//	cout << tmp << endl;
	Intercept::intercept(c1, tmp, C, D);
//	cout << C.x << "  " << C.y << endl;
	
	Intercept::intercept(c4, c5, E, D);
	StrLine tmp2(E, B);
//	cout << tmp2 << endl;
	Intercept::intercept(c2, tmp2, E, D);
//	cout << D.x << "  " << D.y << endl;
	
	
	//~ refPoints.push_back(RefPoint(E, 7));
	//~ refPoints.push_back(RefPoint(G, 7));
	//~ refPoints.push_back(RefPoint(H, 1 << 2));
	
	StrLine rdf(D, C);
	//~ Lgoals.push_back(RefLine(&c3, 3));
	//~ Lgoals.push_back(RefLine(&c4, 3));
	Lgoals.push_back(RefLine(&rdf, 3));

	size_t maxNum = maxLevel * (maxLevel - 1) + refPoints.size() + 100;
	int** RP_Table = new int*[maxNum];
	for (size_t i = 0; i < maxNum; ++i)
	{
		RP_Table[i] = new int[maxNum];
		memset(RP_Table[i], 0, maxNum * sizeof(int));
	}
	/*int n0 = refPoints.size();
	printf("%ld, %ld, %ld, %ld, %ld, %ld, %ld, P %ld\n", N(0, n0), N(1, n0), N(2, n0), N(3, n0), N(4, n0), N(5, n0), N(6, n0),
		N(0, n0) * N(1, n0) * N(2, n0) * N(3, n0) * N(4, n0) * N(5, n0) * N(6, n0));*/

	findLine(Circle(Point(3, 2), Point(4, 2)), 0, refPoints, lines, Lgoals, Pgoals, RP_Table, 0, maxLevel, false);

	steady_clock::time_point t2 = steady_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	std::cout << "It took me " << time_span.count() << " seconds. fcalls " << fcalls << std::endl;
	std::cout.flush();
	printf("End of program\n");

    return 0;
}

