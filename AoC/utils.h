#pragma once
#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <ranges>
#include <unordered_map>
#include <rpc.h>
#include <set>

inline GUID newGuid()
{
	GUID guid;
	UuidCreate(&guid);
	return guid;
}

struct GUIDComparator
{
	bool operator()(const GUID& lhs, const GUID& rhs) const
	{
		return memcmp(&lhs, &rhs, sizeof(GUID)) < 0;
	}
};

template <class T>
using GuidMap = std::map<GUID, T, GUIDComparator>;

using GuidSet = std::set<GUID, GUIDComparator>;


inline std::vector<std::string> split(std::string const& text, std::string const& delimiter)
{
	std::vector<std::string> result;

	int start = 0;
	auto end = text.find(delimiter);
	while (end != std::string::npos)
	{
		auto const found = text.substr(start, end - start);
		if (!found.empty())
			result.push_back(found);
		start = end + delimiter.size();
		end = text.find(delimiter, start);
	}
	auto const rest = text.substr(start, end - start);
	if (!rest.empty())
		result.push_back(text.substr(start, end - start));

	return result;
}

auto range(auto const num)
{
	return std::ranges::iota_view(0, static_cast<int>(num));
}

auto range(auto const start, auto const bound)
{
	return std::ranges::iota_view(static_cast<int>(start), static_cast<int>(bound));
}

class Location : public std::pair<long long, long long>
{
public:
	Location() = default;

	Location(long long y, long long x) :
		std::pair<long long, long long>(y, x)
	{
	}

	[[nodiscard]] Location move(Location const dLocation, long long num = 1) const
	{
		auto move = *this;
		move.first += num * dLocation.first;
		move.second += num * dLocation.second;
		return move;
	}

	[[nodiscard]] Location moveBack(Location const dLocation) const
	{
		auto move = *this;
		move.first -= dLocation.first;
		move.second -= dLocation.second;
		return move;
	}

	[[nodiscard]] long long getDistanceSquared(Location const& other) const
	{
		auto const dx = std::abs(x() - other.x());
		auto const dy = std::abs(y() - other.y());
		return static_cast<long long>(std::pow(dx, 2)) + static_cast<long long>(std::pow(dy, 2));
	}

	[[nodiscard]] long long getManhattenDistance(Location const& other) const
	{
		return std::abs(other.x() - x()) + std::abs(other.y() - y());
	}

	[[nodiscard]] double getDirection(Location const& other) const
	{
		long long const dx = other.x() - x();
		long long const dy = other.y() - y();
		if (dx == 0)
			return DBL_MAX;
		return static_cast<double>(dy) / static_cast<double>(dx);
	}

	[[nodiscard]] long long y() const
	{
		return first;
	}

	[[nodiscard]] long long x() const
	{
		return second;
	}
};

class Coordinate
{
public:
	Coordinate() = default;
	Coordinate(std::string const& value, int x, int y, bool convertToInt):
		location({y, x}),
		strValue(value),
		intValue(convertToInt ? std::stoi(value) : -1)
	{
	}

	int y() const
	{
		return location.y();
	}

	int x() const
	{
		return location.x();
	}

	double getDirection(Coordinate const& other) const
	{
		return location.getDirection(other.location);
	}

	int getDistanceSquared(Coordinate const& other) const
	{
		return location.getDistanceSquared(other.location);
	}

	double getDistance(Coordinate const& other) const
	{
		return std::sqrt(getDistanceSquared(other));
	}

	Location location;
	std::string strValue;
	int intValue;
};

class Grid
{
public:
	Grid(std::vector<std::string> const& input, bool const convertToInt)
	{
		data.resize(input.size());

		int y = 0;
		for (auto const& line : input)
		{
			data[y].resize(line.size());
			int x = 0;
			for (auto const& ch : line)
			{
				data[y][x] = Coordinate{std::string{ch}, x, y, convertToInt};
				++x;
			}
			++y;
		}
	}

	void draw() const
	{
		static int previousLines = 0; // Track the number of lines previously drawn
		int currentLines = data.size() + 2; // Calculate lines this draw will use
		static std::unordered_map<std::string, int> colorMap; // Map characters to colors
		static int colorIndex = 1; // Start assigning colors from 1

		// Hide the cursor
		std::cout << "\033[?25l";

		// Move the cursor up by the number of lines previously drawn
		if (previousLines > 0)
			std::cout << "\033[" << previousLines << "A"; // Move cursor up

		std::cout << "\n";
		for (auto const& line : data)
		{
			for (auto const& coordinate : line)
			{
				// Get the string to print
				std::string value = (coordinate.intValue > -1)
					                    ? std::to_string(coordinate.intValue)
					                    : coordinate.strValue;

				// Assign a color if not already assigned
				if (!colorMap.contains(value))
					colorMap[value] = 31 + (colorIndex++ % 6); // Cycle through 6 colors (31�36)

				// Print the value with its assigned color
				std::cout << "\033[" << colorMap[value] << "m" << value << "\033[0m"; // Reset color after printing
			}
			std::cout << "\n";
		}
		std::cout << "\n";

		// Update the count of previously drawn lines
		previousLines = currentLines;

		// Show the cursor again
		std::cout << "\033[?25h";
	}


	Coordinate& get(int const y, int const x)
	{
		return data[y][x];
	}

	Coordinate& get(const Location& location)
	{
		return data[location.first][location.second];
	}

	bool boundsCheck(Coordinate const& current, Location const& dLocation) const
	{
		return boundsCheck(current, dLocation.first, dLocation.second);
	}

	bool boundsCheck(Coordinate const& current, int const dy, int const dx) const
	{
		auto const yNext = current.y() + dy;
		auto const xNext = current.x() + dx;

		return yNext >= 0 && yNext < yMax() && xNext >= 0 && xNext < xMax();
	}


	Coordinate& get(Coordinate const& current, Location const& dLocation)
	{
		return get(current, dLocation.first, dLocation.second);
	}

	Coordinate& get(Coordinate const& current, int const dy, int const dx)
	{
		return data[current.y() + dy][current.x() + dx];
	}

	Grid mirrorVertical() const
	{
		Grid mirrored{{}, false};
		mirrored.data.resize(data.size());
		int y = 0;
		for (auto const& line : data)
		{
			for (auto const& coordinate : line | std::views::reverse)
			{
				mirrored.data[y].push_back(coordinate);
			}
			++y;
		}
		return mirrored;
	}

	Grid mirrorHorizontal() const
	{
		Grid mirrored{{}, false};
		mirrored.data.resize(yMax());
		for (auto& line : mirrored.data)
			line.resize(xMax());

		for (auto y : range(yMax()))
			for (auto x : range(xMax()))
				mirrored.data[yMax() - y - 1][x] = data[y][x];

		return mirrored;
	}

	Grid transpose() const
	{
		Grid mirrored{{}, false};
		mirrored.data.resize(data[0].size());
		for (auto& line : mirrored.data)
			line.resize(data.size());

		int y = 0;
		for (auto const& line : data)
		{
			int x = 0;
			for (auto const& coordinate : line | std::views::reverse)
			{
				mirrored.data[x][y] = coordinate;
				++x;
			}
			++y;
		}
		return mirrored;
	}

	int xMax() const
	{
		return data[0].size();
	}

	int yMax() const
	{
		return data.size();
	}

	int size() const
	{
		return xMax() * yMax();
	}

	static auto getSurroundingLocations()
	{
		return std::vector<Location>({{-1, 0}, {1, 0}, {0, 1}, {0, -1}});
	}

	auto getSurroundingCoordinates(Coordinate const& coordinate)
	{
		std::vector<Coordinate*> result;
		for (auto const& surroundingLocation : getSurroundingLocations())
			if (boundsCheck(coordinate, surroundingLocation))
				result.push_back(&get(coordinate, surroundingLocation));
		return result;
	}

	std::vector<std::vector<Coordinate>> data;
};

class GridMap
{
public:
	GridMap(std::vector<std::string> const& input, bool const convertToInt)
	{
		int y = 0;
		for (auto const& line : input)
		{
			int x = 0;
			for (auto const& ch : line)
			{
				map[{y, x}] = Coordinate{std::string{ch}, x, y, convertToInt};
				++x;
			}
			++y;
		}
	}

	GridMap(int const yMax, int const xMax, std::string const& fill)
	{
		for (auto const y : range(yMax))
			for (auto const x : range(xMax))
				map[{y, x}] = Coordinate{fill, x, y, false};
	}

	bool boundsCheck(Location const& location) const
	{
		return map.contains(location);
	}

	bool boundsCheck(Location const& current, Location const& move) const
	{
		return boundsCheck(current.move(move));
	}

	static auto getMoves()
	{
		return std::vector<Location>({{-1, 0}, {1, 0}, {0, 1}, {0, -1}});
	}

	auto getSurroundingLocations(Location const& location) const
	{
		std::vector<Location> result;
		for (auto const& surroundingLocation : getMoves())
			if (boundsCheck(location, surroundingLocation))
				result.push_back(surroundingLocation);
		return result;
	}

	long long xMax() const
	{
		for (auto const& location : map | std::views::reverse | std::views::keys)
			return location.x();
		return {};
	}

	long long yMax() const
	{
		for (auto const& location : map | std::views::reverse | std::views::keys)
			return location.y();
		return {};
	}

	void draw() const
	{
		std::cout << "\n";
		for (auto const y : range(yMax() + 1))
		{
			for (auto const x : range(xMax() + 1))
			{
				std::cout << map.at({y, x}).strValue;
			}
			std::cout << "\n";
		}
		return;

		static int previousLines = 0; // Track the number of lines previously drawn
		int currentLines = yMax() + 2; // Calculate lines this draw will use
		static std::unordered_map<std::string, int> colorMap; // Map characters to colors
		static int colorIndex = 1; // Start assigning colors from 1

		// Hide the cursor
		std::cout << "\033[?25l";

		// Move the cursor up by the number of lines previously drawn
		if (previousLines > 0)
			std::cout << "\033[" << previousLines << "A"; // Move cursor up

		std::cout << "\n";
		for (auto const y : range(yMax()))
		{
			for (auto const x : range(xMax()))
			{
				auto const& coordinate = map.at({y, x});
				// Get the string to print
				std::string value = (coordinate.intValue > -1)
					                    ? std::to_string(coordinate.intValue)
					                    : coordinate.strValue;

				// Assign a color if not already assigned
				if (!colorMap.contains(value))
					colorMap[value] = 31 + (colorIndex++ % 6); // Cycle through 6 colors (31�36)

				// Print the value with its assigned color
				std::cout << "\033[" << colorMap[value] << "m" << value << "\033[0m"; // Reset color after printing
			}
			std::cout << "\n";
		}
		std::cout << "\n";

		// Update the count of previously drawn lines
		previousLines = currentLines;

		// Show the cursor again
		std::cout << "\033[?25h";
	}

	std::map<Location, Coordinate> map;
};

template <typename T>
std::vector<std::vector<T>> add(std::vector<T> const& combination, int const numOptions)
{
	std::vector<std::vector<T>> combinations;
	for (auto const i : range(numOptions))
	{
		auto newCombination = combination;
		newCombination.push_back(static_cast<T>(i));
		combinations.emplace_back(newCombination);
	}
	return combinations;
}


template <typename T>
std::vector<std::vector<T>> const& generateCombinations(int const vectorSize, int const numOptions)
{
	static std::map<std::pair<int, int>, std::vector<std::vector<T>>> cache;
	auto const key = std::make_pair(vectorSize, numOptions);
	if (cache.contains(key))
		return cache.at(key);

	std::vector<std::vector<T>> combinations = add(std::vector<T>{}, numOptions);
	while (combinations[0].size() < vectorSize)
	{
		std::vector<std::vector<T>> newCombinations;
		for (auto const& combination : combinations)
			for (auto const& newCombination : add(combination, numOptions))
				newCombinations.emplace_back(newCombination);
		combinations = newCombinations;
	}

	cache[key] = combinations;
	return cache[key];
}

[[nodiscard]] inline bool equal(double a, double b)
{
	return std::abs(a - b) < DBL_EPSILON;
}
