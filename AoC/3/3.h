#pragma once
#include <regex>

#include "Input.h"

class Three : public Puzzle
{
public:
	explicit Three()
		: Puzzle(3)
	{
	}

	int partOne(const Input& input) override
	{
		auto count = 0;
		for (auto const& line : input.lines)
		{
			const std::regex regex{R"(mul\(\d+,\d+\))"};
			auto it = std::sregex_iterator{line.begin(), line.end(), regex};
			for (; it != std::sregex_iterator(); ++it)
			{
				auto const match = it->str();
				bool commaFound = false;
				std::string firstNumStr, secondNumStr;
				for (auto const ch : match | std::views::drop(4))
				{
					if (ch == ',')
					{
						commaFound = true;
						continue;
					}
					if (!std::isdigit(ch))
						continue;
					if (!commaFound)
						firstNumStr += ch;
					else
						secondNumStr += ch;
				}
				//std::cout << std::format("\n{}: {} * {}\n", match, firstNumStr, secondNumStr);
				count += (std::stoi(firstNumStr) * std::stoi(secondNumStr));
			}
		}
		return count;
	}

	int partTwo(const Input& input) override
	{
		const std::string enableStr = "do()";
		const std::string disableStr = "don't()";


		auto modified = input;
		modified.lines.clear();

		std::string oneline;
		for (auto const& line : input.lines)
			oneline += line;

		while (oneline.contains(disableStr))
		{
			auto const disablePos = oneline.find(disableStr);
			auto const enablePos = oneline.find(enableStr);

			if (enablePos < disablePos)
			{
				auto const first = oneline.substr(0, enablePos);
				auto const second = oneline.substr(enablePos + enableStr.size());
				oneline = first + second;
				continue;
			}

			auto const first = oneline.substr(0, disablePos);
			auto const second = oneline.contains(enableStr) ? oneline.substr(enablePos + enableStr.size()) : "";
			oneline = first + second;
		}

		modified.lines.push_back(oneline);

		return partOne(modified);
	}
};
