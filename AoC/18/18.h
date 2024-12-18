#pragma once
#include "Input.h"

struct MemoryRoute
{
	void add(Location const& location)
	{
		assert(order.empty() || location.getManhattenDistance(order.back()) == 1);
		seen.insert(location);
		order.push_back(location);
	}

	std::set<Location> seen;
	std::vector<Location> order;
	bool finished{false};
};

class FallingMemory : public GridMap
{
public:
	FallingMemory(Input const& input, int const yMax, int const xMax) :
		GridMap(yMax, xMax, "."),
		fallingBytes(parseFallingBytes(input)),
		end({yMax - 1, xMax - 1})
	{
	}

	static std::vector<Location> parseFallingBytes(Input const& input)
	{
		std::vector<Location> result;
		for (auto const& line : input.lines)
		{
			auto const parts = split(line, ",");
			auto const x = stoll(parts[0]);
			auto const y = stoll(parts[1]);
			result.emplace_back(y, x);
		}
		return result;
	}

	void simulateFallingBytes(int const num)
	{
		for (auto const i : range(num))
			map[fallingBytes[i]].strValue = "#";
	}

	void addMoveToRoute(MemoryRoute& route, Location const& move) const
	{
		route.add(route.order.back().move(move));
		route.finished = route.order.back() == end;
	}

	GuidMap<MemoryRoute> findShortestRoutes() const
	{
		auto const start = Location{0, 0};
		GuidMap<MemoryRoute> routes;
		std::map<Location, std::pair<GUID, long long>> bestRoutes;
		auto& firstRoute = routes[newGuid()];
		firstRoute.add(start);
		while (!routes.empty())
		{
			std::vector<MemoryRoute> newRoutes;
			GuidSet deadEnds;
			for (auto& [id, route] : routes)
			{
				auto const routeCopy = route;
				auto addAsNewRoute = false;
				auto const& lastLocation = routeCopy.order.back();
				for (auto const& move : getSurroundingLocations(lastLocation))
				{
					auto const newLocation = lastLocation.move(move);
					if (route.finished || route.seen.contains(newLocation) || map.at(newLocation).strValue == "#")
						continue;

					addMoveToRoute(addAsNewRoute ? newRoutes.emplace_back(routeCopy) : route, move);
					addAsNewRoute = true;
				}
				if (!addAsNewRoute && !route.finished)
					deadEnds.insert(id);
			}

			for (auto const& id : deadEnds)
				routes.erase(id);

			for (auto const& route : newRoutes)
				routes[newGuid()] = route;

			GuidSet nonOptimalRoutes;
			for (auto const& [id, route] : routes)
			{
				auto const& lastLocation = route.order.back();
				if (!bestRoutes.contains(lastLocation) || bestRoutes[lastLocation].second > route.order.size())
					bestRoutes[lastLocation] = {id, route.order.size()};
				else if (bestRoutes[lastLocation].first != id)
					nonOptimalRoutes.insert(id);
			}

			for (auto const& id : nonOptimalRoutes)
				routes.erase(id);

			if (std::ranges::all_of(routes | std::views::values, [](auto const& route) { return route.finished; }))
				return routes;
		}
		return {};
	}

	void drawRoute(MemoryRoute const& route)
	{
		auto modified = *this;
		for (auto const& location : route.order)
			modified.map.at(location).strValue = "O";
		modified.draw();
	}

	std::vector<Location> const fallingBytes;
	Location const end;
};

class Eighteen : public Puzzle
{
public:
	explicit Eighteen()
		: Puzzle(18)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto const gridSize = input.lines.size() < 100 ? 7 : 71;
		auto const numBytes = input.lines.size() < 100 ? 12 : 1024;

		auto fallingMemory = FallingMemory{input, gridSize, gridSize};
		fallingMemory.simulateFallingBytes(numBytes);
		fallingMemory.draw();

		auto const shortestRoutes = fallingMemory.findShortestRoutes();
		for (auto const& route : shortestRoutes | std::views::values)
			fallingMemory.drawRoute(route);
		return shortestRoutes.begin()->second.order.size() - 1;
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto const gridSize = input.lines.size() < 100 ? 7 : 71;
		auto fallingMemory = FallingMemory{input, gridSize, gridSize};
		auto numBytes = input.lines.size() < 100 ? 12 : 1024;
		while (true)
		{
			++numBytes;
			std::cout << "\nnumbytes: " << numBytes;
			fallingMemory.simulateFallingBytes(numBytes);
			// We could write another algorithm that stops when a route has been found
			// instead of finding the absolut shortest one every time.
			// But this still runs in a minute or so because it gets faster with each iteration.
			if (fallingMemory.findShortestRoutes().empty())
				break;
		}

		auto const blockage = fallingMemory.fallingBytes[numBytes - 1];
		std::cout << "\n" << blockage.x() << "," << blockage.y() << "\n";

		return numBytes;
	}
};
