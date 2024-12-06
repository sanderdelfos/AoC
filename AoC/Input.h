#pragma once

#include <fstream>
#include <string>

#include "utils.h"

struct Results
{
	int samplePartOne;
	int samplePartTwo;
	int partOne;
	int partTwo;
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

	virtual int partOne(Input const& input) = 0;
	virtual int partTwo(Input const& input) = 0;

	Results getResults()
	{
		return {partOne(sample), partTwo(sample), partOne(test), partTwo(test)};
	}

	Input sample;
	Input test;
};
