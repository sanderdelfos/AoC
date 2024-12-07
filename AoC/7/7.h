#pragma once
#include <bitset>

#include "Input.h"

enum class Operator
{
	Add,
	Multiply,
	Concat,
};

struct Calculation
{
	Calculation(std::string const& line)
	{
		const auto parts = split(line, ": ");
		result = std::stoll(parts[0]);
		for (auto const& num : split(parts[1], " "))
			numbers.push_back(std::stoll(num));
	}

	static long long concat(long long first, long long second)
	{
		return std::stoll(std::to_string(first) + std::to_string(second));
	}

	static long long operate(auto const op, auto const count, auto const num)
	{
		switch (op)
		{
		case Operator::Add:
			return count + num;
		case Operator::Multiply:
			return count * num;
		case Operator::Concat:
			return concat(count, num);
		}
	}

	long long calc(std::vector<Operator> const& combination) const
	{
		auto count = numbers[0];
		for (auto const index : range(numbers.size() - 1))
		{
			auto const op = combination[index];
			auto const& num = numbers[index + 1];
			count = operate(op, count, num);
		}

		return count;
	}

	bool valid() const
	{
		for (auto const& combination : generateCombinations<Operator>(numbers.size(), 2))
			if (calc(combination) == result)
				return true;

		return false;
	}

	bool validP2() const
	{
		for (auto const& combination : generateCombinations<Operator>(numbers.size(), 3))
			if (calc(combination) == result)
				return true;

		return false;
	}

	long long result;
	std::vector<long long> numbers;
};

class Seven : public Puzzle
{
public:
	explicit Seven()
		: Puzzle(7)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto count = 0ll;
		for (auto const& line : input.lines)
		{
			Calculation calculation{line};
			if (calculation.valid())
				count += calculation.result;
		}

		return count;
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;


		auto count = 0ll;
		for (auto const& line : input.lines)
		{
			Calculation calculation{line};
			if (calculation.validP2())
				count += calculation.result;
		}

		return count;
	}
};
