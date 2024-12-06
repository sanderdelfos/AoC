#pragma once
#include "Input.h"

class Two : public Puzzle
{
public:
	explicit Two()
		: Puzzle(2)
	{
	}

	static void parse(const Input& input, std::vector<std::vector<int>>& out)
	{
		for (auto const& line : input.lines)
		{
			auto& outLine = out.emplace_back();
			for (auto const& numStr : split(line, " "))
				outLine.push_back(std::stoi(numStr));
		}
	}

	static bool isSafe(std::vector<int> const& input)
	{
		std::optional<int> prev;
		std::optional<bool> incr;
		for (auto const& cur : input)
		{
			if (!prev.has_value())
			{
				prev = cur;
				continue;
			}

			if (prev == cur)
				return false;

			if (incr.has_value())
			{
				if (*incr && prev > cur)
					return false;
				if (!*incr && prev < cur)
					return false;
			}

			if (std::abs(*prev - cur) > 3)
				return false;

			incr = cur > prev;
			prev = cur;
		}
		return true;
	}

	int partOne(const Input& input) override
	{
		std::vector<std::vector<int>> parsed;
		parse(input, parsed);

		auto count = 0;
		for (auto const& line : parsed)
			if (isSafe(line))
				count++;

		return count;
	}

	static bool isModifiedSafe(std::vector<int> const& line)
	{
		if (isSafe(line))
			return true;

		for (auto i = 0; i < line.size(); i++)
		{
			auto copy = line;
			copy.erase(copy.begin() + i);
			if (isSafe(copy))
				return true;
		}

		return false;
	}

	int partTwo(const Input& input) override
	{
		std::vector<std::vector<int>> parsed;
		parse(input, parsed);

		auto count = 0;
		for (auto const& line : parsed)
			if (isModifiedSafe(line))
				count++;

		return count;
	}
};
