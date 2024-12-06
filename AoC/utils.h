#pragma once
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <ranges>

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
	return std::ranges::iota_view(0, num);
}

using Location = std::pair<int, int>;
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
		return location.first;
	}

	int x() const
	{
		return location.second;
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
		std::cout << "\n";
		for (auto const& line : data)
		{
			for (auto const& coordinate : line)
			{
				std::cout << coordinate.strValue;
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	Coordinate& get(int const y, int const x)
	{
		return data[y][x];
	}

	Coordinate& get(Location location)
	{
		return data[location.first][location.second];
	}

	bool boundsCheck(Coordinate const& current, int const dy, int const dx) const
	{
		auto const yNext = current.y() + dy;
		auto const xNext = current.x() + dx;

		return yNext >= 0 && yNext < yMax() && xNext >= 0 && xNext < xMax();
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

	std::vector<std::vector<Coordinate>> data;
};
