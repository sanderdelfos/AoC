#pragma once
#include <future>

#include "Input.h"

class Program
{
public:
};

class Computer
{
public:
	Computer(Input const& input)
	{
		registerA = std::stoll(split(input.lines[0], "A: ")[1]);
		registerB = std::stoll(split(input.lines[1], "B: ")[1]);
		registerC = std::stoll(split(input.lines[2], "C: ")[1]);
		auto const programStr = split(input.lines[4], ": ")[1];
		for (std::string const& numStr : split(programStr, ","))
			program.push_back(std::stoi(numStr));
	}

	long long combo(long long operand) const
	{
		switch (operand)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			return operand;
		case 4:
			return registerA;
		case 5:
			return registerB;
		case 6:
			return registerC;
		}
		throw std::runtime_error("invalid operand");
	}

	void dv(long long operand, long long& outputRegister)
	{
		auto const numerator = registerA;
		auto const denominator = static_cast<long long>(std::pow(2, combo(operand)));
		if (denominator == 0)
		{
			throw std::runtime_error("invalid operand");
			instructionPointer = -9;
			return;
		}
		outputRegister = numerator / denominator;
	}

	void adv(long long operand)
	{
		dv(operand, registerA);
	}

	void bxl(long long operand)
	{
		registerB = registerB ^ operand;
	}

	void bst(long long operand)
	{
		registerB = combo(operand) % 8;
	}

	void jnz(long long operand)
	{
		if (registerA != 0)
			instructionPointer = operand - 2;
	}

	void bxc(long long operand)
	{
		registerB = registerB ^ registerC;
	}

	void out(long long operand)
	{
		output.push_back(combo(operand) % 8);
	}

	void bdv(long long operand)
	{
		dv(operand, registerB);
	}

	void cdv(long long operand)
	{
		dv(operand, registerC);
	}

	void operate(long long opcode, long long operand)
	{
		switch (opcode)
		{
		case 0:
			return adv(operand);
		case 1:
			return bxl(operand);
		case 2:
			return bst(operand);
		case 3:
			return jnz(operand);
		case 4:
			return bxc(operand);
		case 5:
			return out(operand);
		case 6:
			return bdv(operand);
		case 7:
			return cdv(operand);
		}
		throw std::runtime_error("unsupported opcode");
	}

	void executeProgram()
	{
		while (instructionPointer < program.size() && instructionPointer >= 0)
		{
			operate(program[instructionPointer], program[instructionPointer + 1]);
			instructionPointer += 2;
		}
		for (auto const& value : output | std::views::take(output.size() - 1))
			std::cout << value << ",";
		std::cout << output.back() << "\n";
	}

	static std::string score(std::vector<int> const& in)
	{
		std::string buffer;
		for (auto const& num : in)
			buffer += std::to_string(num);
		return buffer;
	}

	bool isCopy()
	{
		static std::atomic_llong highscore;
		auto const start = registerA;
		auto const programSize = program.size();
		while (instructionPointer < programSize && instructionPointer >= 0)
		{
			operate(program[instructionPointer], program[instructionPointer + 1]);
			instructionPointer += 2;

			if (output.size() > programSize)
				return false;

			for (auto const i : range(output.size()))
				if (program[i] != output[i])
					return false;

			if (output.size() > highscore)
			{
				highscore = output.size();
				std::cout << start << "\n";
			}

			if (output.size() == programSize)
				return true;
		}
		return false;
	}

	long long findCopy()
	{
		auto const original = *this;
		auto count = static_cast<long long>(std::pow(8, program.size() - 1)); // init count to get first number with desired size
		registerA = count;
		while (true)
		{
			auto variant = original;
			variant.registerA = count;
			variant.executeProgram();

			if (program.size() != variant.output.size())
				throw std::runtime_error("we've gone too far...");

			if (program == variant.output)
				return count;

			for (auto const i : range(variant.output.size()) | std::views::reverse)
			{
				if (variant.output[i] != program[i])
				{
					// If you study the output for every starting A, you can notice that the numbers in the output change every 8^pos time.
					// So the first (most left) digit changes every iteration
					// The second digit changes every 8 iterations etc
					// We can use this to speed up the search significantly
					count += static_cast<long long>(std::pow(8, i));
					break;
				}
				
			}
		}
	}

	std::vector<int> program;
	int instructionPointer{0};
	long long registerA;
	long long registerB;
	long long registerC;
	std::vector<int> output;
};


class Seventeen : public Puzzle
{
public:
	explicit Seventeen()
		: Puzzle(17)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto computer = Computer{input};
		computer.executeProgram();
		return -1;
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto computer = Computer{input};
		return computer.findCopy();
	}
};
