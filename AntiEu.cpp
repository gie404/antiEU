// AntiEu.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <iostream>

#include "point.h"
#include "StrLine.h"
#include "Circle.h"
#include "Intercept.h"
#include "CompareLine.h"

uint64_t fcalls = 0;

//struct Hasher
//{
//	std::size_t operator()(const Point& k) const
//	{
//		using std::size_t;
//		using std::hash;
//		
//		return ((hash<long>()((long)round(k.x * invPres))
//			^ (hash<long>()((long)round(k.y * invPres)) << 1)) >> 1);
//	}
//};
//
//struct EqualFn
//{
//	bool operator() (Point const& t1, Point const& t2) const
//	{
//		return t1 == t2;
//	}
//};

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

inline bool checkForPoint(std::vector<RefPoint> &points, Point& p) noexcept
{
	for each (const RefPoint& rp in points)
	{
		if (rp.p == p)
			return true;
	}
	return false;
}

void printMatch(const Line &newLine, std::vector<RefLine> &lines, unsigned level)
{

	printf("Match, level %d, lines %lld -----------------------\n", level, lines.size() + 1);
	for each(const RefLine rline in lines)
	{
	std::cout << *(rline.l) << std::endl;
	}
	std::cout << newLine << std::endl;
	return;
}

inline bool checkForLine(std::vector<RefLine> &lines, Line &newLine) noexcept
{
	for each(const RefLine& rLine in lines)
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
		for each(const RefLine& rLine in lines)
		{
			if (*rLine.l == newLine)
				return;
		}
		
		for each (GoalRefPoint* point in Pgoals)
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

		
		for each(const RefLine& rline in lines)
		{
			RefPoint a, b;
			unsigned num = Intercept::intercept(*rline.l, newLine, a.p, b.p);

			bool newA = num > 0;
			bool newB = num > 1;
			for each(const RefPoint &rp in refPoints)
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

int main()
{
	using namespace std::chrono;
	
	steady_clock::time_point t1 = steady_clock::now();

	std::vector<RefPoint> refPoints;
	std::vector<RefLine> lines;
	std::vector<RefLine> Lgoals;
	std::vector<GoalRefPoint*> Pgoals;
	const unsigned maxLevel = 3;

	refPoints.reserve(10000);
	lines.reserve(10000);
	
	//Point A(0, 0), D, E, F, G, H;
	//Point B(3.72, 5);
	//Point C(2, 0);
	//D = (B + C) / 3;

	//StrLine rl1(A, B);
	//StrLine rl2(A, C);

	//refPoints.push_back(RefPoint(A, 3));
	//refPoints.push_back(RefPoint(D, 4));
	////refPoints.push_back(RefPoint(C, 4));
	//lines.push_back(RefLine(&rl1, 1));
	//lines.push_back(RefLine(&rl2, 2));
	//
	//StrLine rl3(A, D);
	//lines.push_back(RefLine(&rl3, 7));
	///*Circle c1(A, D);
	//lines.push_back(RefLine(&c1, 7));
	//Intercept::intercept(rl1, c1, E, G);
	//refPoints.push_back(RefPoint(E, 7));
	//Intercept::intercept(rl2, c1, E, G);
	//refPoints.push_back(RefPoint(E, 7));*/
	////Pgoals.push_back(new GoalRefPoint(B * B.norm() / C.norm(), 7, true));
	//
	//Circle c1(D, A);
	//Circle c2(2 * D, D);
	//Intercept::intercept(rl1, c2, E, G);
	//Circle c3(A, G);
	////Intercept::intercept(rl3, c3, E, G);
	//Intercept::intercept(c2, c3, F, H);

	//Point X = D * 3 / 2;
	//StrLine rl4(E, H);
	//Intercept::intercept(rl4, c1, F, H);
	//Lgoals.push_back(RefLine(new StrLine(B, C), 7));

	//---------------------------------------------------------------------------
	//StrLine rl1(Point(0, -1), 1);
	//StrLine rl2(Point(0.627, sqrt(1 - 0.627 *0.627)), 1);
	//StrLine rl3(Point(-0.41, sqrt(1 - 0.41 * 0.41)), 1);
	//Point A, B, C, D, E, F, G;
	//Intercept::intercept(rl1, rl2, D, G);
	//Intercept::intercept(rl1, rl3, E, G);
	//Intercept::intercept(rl3, rl2, F, G);

	//refPoints.push_back(RefPoint(D, 1));
	//refPoints.push_back(RefPoint(E, 2));
	//refPoints.push_back(RefPoint(F, 4));

	////refPoints.push_back(RefPoint(Point(0, 0), 7));

	//lines.push_back(RefLine(&rl1, 3));
	//lines.push_back(RefLine(&rl2, 5));
	//lines.push_back(RefLine(&rl3, 6));

	////Circle c1(D, F);
	////Circle c2(F, D);
	////lines.push_back(RefLine(&c1, 5));
	////lines.push_back(RefLine(&c2, 5));

	////Intercept::intercept(c1, rl1, A, B);
	////refPoints.push_back(RefPoint(A, 7));
	/////*Intercept::intercept(c1, rl3, A, B);
	////refPoints.push_back(RefPoint(A, 7));*/
	////Intercept::intercept(c2, rl3, A, B);
	////refPoints.push_back(RefPoint(B, 7));
	////Intercept::intercept(c2, rl1, A, B);
	////refPoints.push_back(RefPoint(A, 7));

	////Intercept::intercept(c1, c2, A, B);
	////refPoints.push_back(RefPoint(A, 5));
	////refPoints.push_back(RefPoint(B, 5));

	//Circle nc(Point(0, 0), 1);
	//Intercept::intercept(nc, rl2, A, B);
	////Lgoals.push_back(RefLine(&nc, 7));
	////Pgoals.push_back(new GoalRefPoint(Point(0, 0), 7, false));
	//Pgoals.push_back(new GoalRefPoint(A, 7, true));


	//---------------------------------------------------------------------------
	Point A(0, 0), B(1, 0), C(6, 0);
	refPoints.push_back(RefPoint(A, 1));
	refPoints.push_back(RefPoint(C, 2));
	lines.push_back(RefLine(new StrLine(A, C), 3));

	refPoints.push_back(RefPoint( (A + C)/2, 3));
	lines.push_back(RefLine(new StrLine(B, 3), 3));

	refPoints.push_back(RefPoint((A + C) * 3 / 4, 3));
	lines.push_back(RefLine(new StrLine(B, 4.5), 3));

	GoalRefPoint neededPoint(B, 3, true);
	
	Pgoals.push_back(&neededPoint);

	
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
	//findLine(Circle(Point(3, 2), Point(4, 2)), 0, false);

	steady_clock::time_point t2 = steady_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	std::cout << "It took me " << time_span.count() << " seconds. fcalls " << fcalls << std::endl;
	std::cout.flush();
	printf("End of program\n");

	getchar();
    return 0;
}

