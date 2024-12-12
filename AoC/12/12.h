#pragma once
#include "Input.h"

class Farm : public Grid
{
public:
	Farm(std::vector<std::string> const& input)
		: Grid(input, false)
	{
		std::set<Location> seen;
		for (auto const& line : data)
		{
			for (auto const& coordinate : line)
			{
				if (seen.contains(coordinate.location))
					continue;


				std::set<Location> group{{coordinate.location}};
				expandGroup(coordinate, group);
				seen.insert_range(group);
				regions.push_back(group);
			}
		}
	}

	void expandGroup(Coordinate const& coordinate, std::set<Location>& group)
	{
		for (auto const& neighbour : getSurroundingCoordinates(coordinate))
		{
			if (group.contains(neighbour->location) || coordinate.strValue != neighbour->strValue)
				continue;

			group.insert(neighbour->location);
			expandGroup(*neighbour, group);
		}
	}

	long long getFencingPriceForRegion(std::set<Location> const& region)
	{
		auto numFences = 0ll;
		std::string regionStr;
		for (auto const& location : region)
		{
			auto const& coordinate = get(location);
			regionStr = coordinate.strValue;
			auto numNeigbours = 0;
			for (auto const& neighbour : getSurroundingCoordinates(coordinate))
				if (!region.contains(neighbour->location))
					++numNeigbours;
			if (coordinate.x() == 0 || coordinate.x() == (xMax() - 1))
				++numNeigbours;
			if (coordinate.y() == 0 || coordinate.y() == (yMax() - 1))
				++numNeigbours;
			numFences += numNeigbours;
		}

		std::cout << std::format("\nRegion {} of size {} has {} fences\n", regionStr, region.size(), numFences);

		return numFences * region.size();
	}

	long long getTotalFencingPrice()
	{
		auto price = 0ll;
		for (auto const& region : regions)
			price += getFencingPriceForRegion(region);
		return price;
	}

	long long getDiscountedFencingPriceForRegion(std::set<Location> const& region)
	{
		std::map<Location, Location> verticalFences;
		std::map<Location, Location> horizontalFences;

		for (auto const& location : region)
		{
			auto& coordinate = get(location);

			auto check = [&](auto& set, auto const dLocation)
			{
				auto const newLocation = location.move(dLocation);
				if (!boundsCheck(coordinate, dLocation) || !region.contains(newLocation))
				{
					auto moveFenceLocation = dLocation;
					if (moveFenceLocation.first < 0)
						moveFenceLocation.first = 0;
					if (moveFenceLocation.second < 0)
						moveFenceLocation.second = 0;
					set[location.move(moveFenceLocation)] = dLocation;
				}
			};

			check(horizontalFences, Location{-1, 0});
			check(horizontalFences, Location{1, 0});
			check(verticalFences, Location{0, 1});
			check(verticalFences, Location{0, -1});
		}

		auto sidesCount = 0ll;


		for (auto const x : range(-1, xMax() + 1))
		{
			bool line = false;
			for (auto const y : range(-1, yMax() + 1))
			{
				auto const location = Location{y, x};
				auto const prevLocation = location.move({-1, 0});
				if (!verticalFences.contains(location))
				{
					line = false;
					continue;
				}

				if (line && verticalFences[location] != verticalFences[prevLocation])
					++sidesCount; // new line in other direction

				if (!line)
					++sidesCount;
				line = true;
			}
		}

		for (auto const y : range(-1, yMax() + 1))
		{
			bool line = false;
			for (auto const x : range(-1, xMax() + 1))
			{
				auto const location = Location{y, x};
				auto const prevLocation = location.move({0, -1});
				if (!horizontalFences.contains(location))
				{
					line = false;
					continue;
				}

				if (line && horizontalFences[location] != horizontalFences[prevLocation])
					++sidesCount; // new line in other direction

				if (!line)
					++sidesCount;
				line = true;
			}
		}

		return sidesCount * region.size();
	}

	long long getTotalDiscountedFencingPrice()
	{
		auto price = 0ll;
		for (auto const& region : regions)
			price += getDiscountedFencingPriceForRegion(region);
		return price;
	}

	void drawIds() const
	{
		std::cout << "\n\n";
		for (auto& line : data)
		{
			for (auto& coordinate : line)
			{
				std::cout << coordinate.strValue << coordinate.intValue << "\t";
			}
			std::cout << "\n";
		}
		std::cout << "\n\n";
	}

	std::vector<std::set<Location>> regions;
};


class Twelve : public Puzzle
{
public:
	explicit Twelve()
		: Puzzle(12)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		Farm farm{input.lines};
		//farm.drawIds();

		return farm.getTotalFencingPrice();
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		Farm farm{input.lines};


		return farm.getTotalDiscountedFencingPrice();
	}
};
