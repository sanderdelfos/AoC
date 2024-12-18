// AoC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "1/1.h"
#include "10/10.h"
#include "11/11.h"
#include "12/12.h"
#include "13/13.h"
#include "14/14.h"
#include "15/15.h"
#include "16/16.h"
#include "17/17.h"
#include "18/18.h"
#include "2/2.h"
#include "3/3.h"
#include "4/4.h"
#include "5/5.h"
#include "6/6.h"
#include "7/7.h"
#include "8/8.h"
#include "9/9.h"

std::unique_ptr<Puzzle> getPuzzle(int const day)
{
	switch (day)
	{
	case 1:
		return std::make_unique<One>();
	case 2:
		return std::make_unique<Two>();
	case 3:
		return std::make_unique<Three>();
	case 4:
		return std::make_unique<Four>();
	case 5:
		return std::make_unique<Five>();
	case 6:
		return std::make_unique<Six>();
	case 7:
		return std::make_unique<Seven>();
	case 8:
		return std::make_unique<Eight>();
	case 9:
		return std::make_unique<Nine>();
	case 10:
		return std::make_unique<Ten>();
	case 11:
		return std::make_unique<Eleven>();
	case 12:
		return std::make_unique<Twelve>();
	case 13:
		return std::make_unique<Thirteen>();
	case 14:
		return std::make_unique<Fourteen>();
	case 15:
		return std::make_unique<Fifteen>();
	case 16:
		return std::make_unique<Sixteen>();
	case 17:
		return std::make_unique<Seventeen>();
	case 18:
		return std::make_unique<Eighteen>();
	}

	return {};
}

int main()
{
	std::cout << "Day?\n";
	int day;
	std::cin >> day;
	auto const results = getPuzzle(day)->getResults();
	std::cout << "Part one Sample result for day " << day << " is " << results.samplePartOne << "\n";
	std::cout << "Part one Result for day " << day << " is " << results.partOne << "\n";
	std::cout << "Part two Sample result for day " << day << " is " << results.samplePartTwo << "\n";
	std::cout << "Part two Result for day " << day << " is " << results.partTwo << "\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
