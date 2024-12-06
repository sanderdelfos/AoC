#pragma once

#include <algorithm>
#include <vector>
#include <ranges>
#include "input.h"

class One : public Puzzle
{
public:
	One() : Puzzle(1)
	{
	}

	void parse(auto const& input, auto& left, auto& right)
	{
		for (auto const& line : input)
		{
			auto splitLine = split(line, "   ");
			left.push_back(std::stoi(splitLine[0]));
			right.push_back(std::stoi(splitLine[1]));
		}
	}

	int partOne(const Input& input) override
	{
		std::vector<int> left;
		std::vector<int> right;

		parse(input.lines, left, right);

		std::ranges::sort(left);
		std::ranges::sort(right);

		auto const size = left.size();
		auto distance = 0;
		for (auto i = 0; i < size; i++)
		{
			distance += std::abs(left[i] - right[i]);
		}
		return distance;
	}

	int partTwo(const Input& input) override
	{
		std::vector<int> left;
		std::vector<int> right;

		parse(input.lines, left, right);

		auto count = 0;
		for (auto const& num : left)
			count += num * std::ranges::count(right, num);
		return count;
	}
};
