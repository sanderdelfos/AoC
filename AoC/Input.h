#pragma once

#include <fstream>
#include <string>

#include "utils.h"

struct Results
{
	long long samplePartOne;
	long long samplePartTwo;
	long long partOne;
	long long partTwo;
};


class Input
{
public:
	Input(int const day, bool const sample)
	{
		std::string line;
		std::ifstream input;
		auto const filename = std::format("{}/{}{}.txt", day, day, sample ? "Sample" : "");
		input.open(filename);
		while (std::getline(input, line))
			lines.push_back(line);
	}

	std::vector<std::string> lines;
};

class Puzzle
{
public:
	Puzzle(int const day):
		sample(day, true),
		test(day, false)
	{
	}

	virtual long long partOne(Input const& input) = 0;
	virtual long long partTwo(Input const& input) = 0;

	Results getResults()
	{
		return {partOne(sample), partTwo(sample), partOne(test), partTwo(test)};
	}

	Input sample;
	Input test;
};
