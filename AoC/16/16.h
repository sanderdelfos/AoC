#pragma once
#include "Input.h"

struct Route
{
	Route(Location const& location, Location const& move) :
		current(location),
		direction(move)
	{
		seen[location] = points;
	}

	Route() = default;

	void add(Location const& location, Location const& move)
	{
		if (move != direction)
			points += 1000ll;
		++points;
		seen[location] = points;
		direction = move;
		current = location;
	}

	long long points{0};
	Location current;
	Location direction;
	std::map<Location, long long> seen;
	bool finished{false};
};

class ReindeerMaze : public GridMap
{
public:
	ReindeerMaze(const Input& input) :
		GridMap(input.lines, false),
		idCursor(0),
		bestId(LLONG_MAX)
	{
		for (auto const& coordinate : map | std::views::values)
		{
			if (coordinate.strValue == "S")
				start = coordinate.location;
			if (coordinate.strValue == "E")
				end = coordinate.location;
		}
		routes[idCursor++] = Route{start, {0, 1}};
	}

	void addRoutes(Route const& route)
	{
		if (route.finished)
		{
			routes[idCursor++] = route;
			return;
		}

		for (auto const& move : getMoves())
		{
			if (!boundsCheck(route.current, move))
				continue;

			auto const& surroundingLocation = route.current.move(move);
			if (map.at(surroundingLocation).strValue == "#" || route.seen.contains(surroundingLocation))
				continue;

			auto& newRoute = routes[idCursor++];
			newRoute = route;
			newRoute.add(surroundingLocation, move);
			if (surroundingLocation == end)
				newRoute.finished = true;

		}
	}

	bool isBetterRoute(Route const& route) const
	{
		if (!locationToDirectionToHighscore.contains(route.current))
			return true;
		if (!locationToDirectionToHighscore.at(route.current).contains(route.direction))
			return true;
		return locationToDirectionToHighscore.at(route.current).at(route.direction) >= route.points;
	}

	void expandRoutes()
	{
		while (true)
		{
			std::set<long long> toDelete;
			long long highscore = LLONG_MAX;
			auto const routesCopy = routes;
			for (auto& [id, route] : routesCopy)
			{
				toDelete.insert(id); // We insert new routes so the old can be removed
				addRoutes(route);
			}

			//for (auto const& newRoute : toInsert)
			//	routes[idCursor++] = newRoute;

			for (auto const& [id, route] : routes)
			{
				if (isBetterRoute(route))
					locationToDirectionToHighscore[route.current][route.direction] = route.points;
				else
					toDelete.insert(id);

				if (route.finished)
				{
					if (route.points > highscore)
						toDelete.insert(id);
					else
						highscore = route.points;
				}
			}

			for (auto const& id : toDelete)
				routes.erase(id);

			//std::cout << routes.size() << "\n";

			if (std::ranges::all_of(routes | std::views::values, [](Route const& route) { return route.finished; }))
				return;
		}
	}

	Route const& getBestRoute()
	{
		expandRoutes();
		return routes.begin()->second;
	}

	void drawRoute(Route const& route) const
	{
		std::cout << "\n";
		for (auto const y : range(yMax() + 1))
		{
			for (auto const x : range(xMax() + 1))
			{
				if (route.seen.contains({y, x}))
					std::cout << "X";
				else
					std::cout << map.at({y, x}).strValue;
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	Route combinedRoutes() const
	{
		Route combined;
		for (auto const& route : routes | std::views::values)
			combined.seen.insert_range(route.seen);
		return combined;
	}

	Location end;
	Location start;
	std::map<long long, Route> routes;
	std::map<Location, std::map<Location, long long>> locationToDirectionToHighscore;
	long long idCursor;
	long long bestId;
};

class Sixteen : public Puzzle
{
public:
	explicit Sixteen()
		: Puzzle(16)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto maze = ReindeerMaze{input};
		auto const bestRoute = maze.getBestRoute();
		//maze.drawRoute(bestRoute);
		return bestRoute.points;
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto maze = ReindeerMaze{input};
		maze.expandRoutes();
		//for (auto const& route : maze.routes | std::views::values)
		//	maze.drawRoute(route);

		return maze.combinedRoutes().seen.size();
	}
};
