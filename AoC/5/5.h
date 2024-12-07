#pragma once
#include <set>
#include <unordered_set>

#include "Input.h"

struct FiveData
{
	std::set<std::pair<int, int>> ordering;
	std::vector<std::vector<int>> pagesList;
};

class Five : public Puzzle
{
public:
	explicit Five()
		: Puzzle(5)
	{
	}

	FiveData parse(Input const& input)
	{
		FiveData fiveData;

		std::vector<std::string> first;
		std::vector<std::string> second;


		for (auto const& line : input.lines)
		{
			if (second.empty() && !line.empty())
				first.emplace_back(line);
			else
				second.emplace_back(line);
		}

		for (auto const& line : first)
		{
			auto const parts = split(line, "|");
			fiveData.ordering.insert(std::make_pair(std::stoi(parts[0]), std::stoi(parts[1])));
		}

		for (auto const& line : second)
		{
			if (line.empty())
				continue;

			auto const parts = split(line, ",");
			auto& lineData = fiveData.pagesList.emplace_back();
			for (auto const& part : parts)
				lineData.push_back(std::stoi(part));
		}

		return fiveData;
	}

	static bool isValid(FiveData const& data, std::vector<int> const& pages)
	{
		auto valid = true;
		for (auto const index : range(static_cast<int>(pages.size())))
		{
			auto const& startPage = pages[index];
			for (auto followingPageIndex = index + 1; followingPageIndex < pages.size(); ++followingPageIndex)
			{
				auto const& followingPage = pages[followingPageIndex];
				valid = valid && data.ordering.contains(std::make_pair(startPage, followingPage));
			}
		}
		return valid;
	}

	long long partOne(const Input& input) override
	{
		auto const parsed = parse(input);

		auto count = 0;
		for (auto const& pages : parsed.pagesList)
		{
			if (isValid(parsed, pages))
			{
				auto const middlePage = pages[static_cast<int>(pages.size() / 2)];
				count += middlePage;
			}
		}

		return count;
	}

	long long partTwo(const Input& input) override
	{
		auto const parsed = parse(input);
		auto count = 0;
		for (auto const& pages : parsed.pagesList)
		{
			if (isValid(parsed, pages))
				continue;

			auto orderedPages = pages;
			std::ranges::sort(orderedPages, [&](auto const first, auto const second)
			{
				return parsed.ordering.contains(std::make_pair(first, second));
			});

			auto const middlePage = orderedPages[static_cast<int>(orderedPages.size() / 2)];
			count += middlePage;
		}

		return count;
	}
};
