// ReSharper disable CppInconsistentNaming
#pragma once
#include "Input.h"

class AntennaMap : public Grid
{
public:
	AntennaMap(std::vector<std::string> const& input, bool convertToInt)
		: Grid(input, convertToInt)
	{
		for (auto const& line : data)
		{
			for (auto const& coordinate : line)
			{
				if (coordinate.strValue == ".")
					continue;

				antennas.push_back(coordinate);
			}
		}
	}

	virtual std::set<Location> calcAntinodes(Coordinate const& first, Coordinate const& second)
	{
		std::set<Location> antinodes;
		for (auto const x : range(xMax()))
		{
			for (auto const y : range(yMax()))
			{
				auto const& possibleAntinode = get(y, x);
				if (std::abs((first.getDirection(possibleAntinode) - possibleAntinode.getDirection(second))) >
					DBL_EPSILON)
					continue;

				auto const firstDistance = first.getDistance(possibleAntinode);
				auto const secondDistance = second.getDistance(possibleAntinode);
				if (std::abs((2 * firstDistance - secondDistance)) < DBL_EPSILON || std::abs(
					(2 * secondDistance - firstDistance)) < DBL_EPSILON)
					antinodes.insert(possibleAntinode.location);
			}
		}
		return antinodes;
	}

	std::set<Location> findAntinodes()
	{
		std::set<Location> antinodes;
		for (auto const& firstAntenna : antennas)
		{
			for (auto const& secondAntenna : antennas)
			{
				if (firstAntenna.location == secondAntenna.location || firstAntenna.strValue != secondAntenna.strValue)
					continue;
				antinodes.insert_range(calcAntinodes(firstAntenna, secondAntenna));
			}
		}
		return antinodes;
	}

	void drawAntinodes(std::set<Location> const& antinodes) const
	{
		auto copy = *this;
		for (auto const& antinode : antinodes)
			copy.get(antinode).strValue = "#";
		copy.draw();
	}

	std::vector<Coordinate> antennas;
};

class HarmonicAntennaMap : public AntennaMap
{
public:
	HarmonicAntennaMap(std::vector<std::string> const& input, bool convertToInt)
		: AntennaMap(input, convertToInt)
	{
	}

	std::set<Location> calcAntinodes(Coordinate const& first, Coordinate const& second) override
	{
		std::set<Location> antinodes;
		antinodes.insert(first.location);
		antinodes.insert(second.location);

		for (auto const x : range(xMax()))
		{
			for (auto const y : range(yMax()))
			{
				auto const& possibleAntinode = get(y, x);
				if (std::abs((first.getDirection(possibleAntinode) - possibleAntinode.getDirection(second))) < 0.00001)
					antinodes.insert(possibleAntinode.location);
			}
		}
		return antinodes;
	}
};

class Eight : public Puzzle
{
public:
	explicit Eight()
		: Puzzle(8)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto grid = AntennaMap{input.lines, false};
		auto const antinodes = grid.findAntinodes();
		grid.drawAntinodes(antinodes);
		return antinodes.size();
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto grid = HarmonicAntennaMap{input.lines, false};
		auto const antinodes = grid.findAntinodes();
		grid.drawAntinodes(antinodes);
		return antinodes.size();
	}
};
