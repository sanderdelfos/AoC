#pragma once
#include <cassert>
#include <unordered_set>

#include "Input.h"


class Stone
{
public:
	bool operator==(const Stone& other) const
	{
		return other.value == value;
	}

	std::vector<Stone> blink() const
	{
		if (value == 0)
			return {{1}};

		auto const strValue = std::to_string(value);
		auto const size = strValue.size();

		if (size % 2 == 0)
		{
			auto const left = stoll(strValue.substr(0, size / 2));
			auto const right = stoll(strValue.substr(size / 2));
			return {{left}, {right}};
		}

		return {{value * 2024}};
	}

	static std::vector<Stone> blink(std::vector<Stone> const& stones)
	{
		std::vector<Stone> newStones;
		for (auto const& stone : stones)
			for (auto const& newStone : stone.cachedBlink())
				newStones.push_back(newStone);
		return newStones;
	}

	std::vector<Stone> const& cachedBlink() const
	{
		static std::unordered_map<long long, std::vector<Stone>> cache{};
		auto& cacheItem = cache[value];
		if (!cacheItem.empty())
			return cacheItem;

		cacheItem = blink();
		return cacheItem;
	}

	long long value;
};

inline auto parse(Input const& input)
{
	std::vector<Stone> result;
	for (auto const& part : split(input.lines[0], " "))
		result.push_back({stoll(part)});
	return result;
}

class StoneCache
{
public:
	static void init(int const blinks)
	{
		for (auto const calcBlinks : range(1, blinks + 1))
		{
			if (map.contains(calcBlinks))
				continue;

			for (auto const startNum : range(10))
			{
				std::cout << startNum << calcBlinks << std::endl;
				calc(startNum, calcBlinks);
			}
		}
	}

	static void calc(int startNum, int blinks)
	{
		auto stones = std::vector({Stone{startNum}});
		auto& cache = map[startNum];
		auto carry = 0ll;
		for (auto const blink : range(1, blinks + 1))
		{
			auto foundInCache = std::unordered_set<long long>{};
			auto const remainingBlinks = 1 + blinks - blink;
			for (auto const& stone : stones)
			{
				if (map.contains(stone.value) && map[stone.value].contains(remainingBlinks))
				{
					carry += map[stone.value].at(remainingBlinks);
					foundInCache.insert(stone.value);
				}
			}

			for (auto const& stoneValue : foundInCache)
				std::erase(stones, Stone{stoneValue});

			stones = Stone::blink(stones);
			if (carry == 0ll)
				cache[blink] = stones.size();
		}
		cache[blinks] = carry + stones.size();
	}

	static inline std::unordered_map<long long, std::unordered_map<int, long long>> map{};
};

class Stones
{
public:
	using ListType = std::vector<std::vector<Stone> const*>;

	Stones(Input const& input) :
		originalList(parse(input))
	{
	}

	void blink()
	{
		originalList = Stone::blink(originalList);
	}

	long long cachedBlink(int blinks)
	{
		StoneCache::init(blinks);
		auto carry = 0ll;
		for (auto const i : range(1, blinks + 1))
		{
			std::unordered_set<long long> foundInCache;
			auto const remainingBlinks = 1 + blinks - i;
			for (auto const& stone : originalList)
			{
				if (StoneCache::map.contains(stone.value) && StoneCache::map.at(stone.value).contains(remainingBlinks))
				{
					foundInCache.insert(stone.value);
					carry += StoneCache::map.at(stone.value).at(remainingBlinks);
					assert(carry > 0);
				}
			}

			for (auto const& toRemove : foundInCache)
				std::erase(originalList, Stone{toRemove});

			originalList = Stone::blink(originalList);
		}

		return carry + originalList.size();
	}

	void draw() const
	{
		std::cout << "\n\n";
		for (auto const& stone : originalList)
			std::cout << stone.value << " ";
		std::cout << "\n\n";
	}

	long long size() const
	{
		return originalList.size();
	}


	std::vector<Stone> originalList;
};

class Eleven : public Puzzle
{
public:
	explicit Eleven()
		: Puzzle(11)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto stones = Stones{input};

		// Slow method also works for part one...
		//for (auto const i : range(25))
		//{
		//	stones.blink();
		//	//stones.draw();
		//}
		// return stones.size();
		// But this is must faster:
		return stones.cachedBlink(25);
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		// Part two needs some clever caching
		auto stones = Stones{input};
		return stones.cachedBlink(75);
	}
};
