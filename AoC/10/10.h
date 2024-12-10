#pragma once
#include <set>

#include "Input.h"


class TopographicMap : public Grid
{
public:
	TopographicMap(std::vector<std::string> const& input)
		: Grid(input, true)
	{
	}

	std::vector<std::vector<Location>> getTrailheads()
	{
		std::vector<std::vector<Location>> trailheads;
		for (auto const y : range(yMax()))
			for (auto const x : range(xMax()))
				if (get(y, x).intValue == 0)
					trailheads.push_back({get(y, x).location});
		return trailheads;
	}

	std::vector<Location> getMoves(Coordinate const& current)
	{
		std::vector<Location> moves;
		std::vector<Location> const steps = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

		for (auto const& step : steps)
			if (boundsCheck(current, step) && get(current, step).intValue == (current.intValue + 1))
				moves.push_back(get(current, step).location);
		return moves;
	}

	bool trailFinished(std::vector<Location> const& trail)
	{
		return get(trail.back()).intValue == 9;
	}


	bool trailsFinished(std::vector<std::vector<Location>> const& trails)
	{
		for (auto const& trail : trails)
			if (!trailFinished(trail))
				return false;
		return true;
	}

	std::vector<std::vector<Location>> getMoves(std::vector<std::vector<Location>> const& trails)
	{
		std::vector<std::vector<Location>> newTrails;
		for (auto const& trail : trails)
		{
			if (trailFinished(trail))
			{
				newTrails.push_back(trail);
				continue;
			}

			for (auto const& move : getMoves(get(trail.back())))
			{
				auto newTrail = trail;
				newTrail.push_back(move);
				newTrails.push_back(newTrail);
			}
		}
		return newTrails;
	}

	std::vector<std::vector<Location>> getTrails()
	{
		std::vector<std::vector<Location>> trails = getTrailheads();
		while (!trailsFinished(trails))
			trails = getMoves(trails);
		return trails;
	}

	int getScore()
	{
		std::map<Location, std::set<Location>> trailheadToEnds;
		for (auto const& trail : getTrails())
			trailheadToEnds[trail.front()].insert(trail.back());

		int score = 0;
		for (auto const& ends : trailheadToEnds | std::views::values)
			score += ends.size();

		return score;
	}

	int getRating()
	{
		return getTrails().size();
	}
};


class Ten : public Puzzle
{
public:
	explicit Ten()
		: Puzzle(10)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto topographicMap = TopographicMap{input.lines};
		return topographicMap.getScore();
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto topographicMap = TopographicMap{input.lines};
		return topographicMap.getRating();
	}
};
