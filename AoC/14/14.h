#pragma once
#include "Input.h"

class Robot
{
public:
	Robot(std::string const& input)
	{
		auto const parts = split(input, " ");
		auto const locationParts = split(parts[0].substr(2), ",");
		location = {stoll(locationParts[1]), stoll(locationParts[0])};
		auto const velocityParts = split(parts[1].substr(2), ",");
		velocity = {stoll(velocityParts[1]), stoll(velocityParts[0])};
	}

	Location location;
	Location velocity;
};


class BathroomSecurity
{
public:
	BathroomSecurity(const Input& input, int width, int height)
		: grid(createInput(width, height), false)
	{
		for (auto const& line : input.lines)
			robots.emplace_back(line);
	}

	static std::vector<std::string> createInput(int width, int height)
	{
		std::string str;
		str.resize(width, '.');
		return std::vector(height, str);
	}

	void moveRobot(Robot& robot) const
	{
		robot.location = robot.location.move(robot.velocity);

		if (robot.location.x() < 0)
			robot.location = robot.location.move(Location{0, grid.xMax()});
		if (robot.location.x() >= grid.xMax())
			robot.location = robot.location.moveBack(Location{0, grid.xMax()});
		if (robot.location.y() < 0)
			robot.location = robot.location.move(Location{grid.yMax(), 0});
		if (robot.location.y() >= grid.yMax())
			robot.location = robot.location.moveBack(Location{grid.yMax(), 0});
		assert(robot.location.x() > -1 && robot.location.y() > -1);
	}

	void moveRobots()
	{
		for (auto& robot : robots)
			moveRobot(robot);
	}

	void moveRobots(long long seconds)
	{
		for (auto _ : range(seconds))
			moveRobots();
	}


	void drawRobots() const
	{
		auto robotGrid = grid;
		for (auto const& robot : robots)
		{
			auto& coordinate = robotGrid.get(robot.location);
			if (coordinate.intValue == -1)
				coordinate.intValue = 1;
			else
				++coordinate.intValue;
		}
		robotGrid.draw();
	}

	long long safetyFactor() const
	{
		auto const xMiddle = grid.xMax() / 2;
		auto const yMiddle = grid.yMax() / 2;

		std::array<long long, 4> quadrantScores;
		for (auto const xQuadrant : {0, 1})
		{
			for (auto const yQuadrant : {0, 1})
			{
				auto& quadrantScore = quadrantScores[xQuadrant + 2 * yQuadrant];
				quadrantScore = 0;
				auto inQuadrant = [&](Robot const& robot)
				{
					if (robot.location.x() == xMiddle || robot.location.y() == yMiddle)
						return false;

					return robot.location.x() >= xQuadrant * xMiddle && robot.location.x() <= (1 + xQuadrant) * xMiddle
						&&
						robot.location.y() >= yQuadrant * yMiddle && robot.location.y() <= (1 + yQuadrant) * yMiddle;
				};

				for (auto const& robot : robots)
					if (inQuadrant(robot))
						++quadrantScore;
			}
		}

		auto safetyScore = quadrantScores[0];
		for (auto const& quadrantScore : quadrantScores | std::views::drop(1))
			safetyScore *= quadrantScore;
		return safetyScore;
	}

	bool hasAdjacentRobot(Robot const& robot) const
	{
		for (auto const& other : robots)
		{
			if (robot.location == other.location)
				continue;

			if (robot.location.getManhattenDistance(other.location) == 1)
				return true;
		}
		return false;
	}

	bool maybeTree() const
	{
		auto const numRobots = robots.size();
		auto numAdjacentRobots = 0;
		for (auto const& robot : robots)
			if (hasAdjacentRobot(robot))
				++numAdjacentRobots;

		return (2 * numAdjacentRobots) > numRobots;
	}

	std::vector<Robot> robots;
	Grid const grid;
};


class Fourteen : public Puzzle
{
public:
	explicit Fourteen()
		: Puzzle(14)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		if (input.lines.size() < 100)
		{
			auto small = BathroomSecurity{input, 11, 7};
			small.drawRobots();
			small.moveRobots(100);
			small.drawRobots();
			return small.safetyFactor();
		}

		auto bathroomSecurity = BathroomSecurity{input, 101, 103};
		bathroomSecurity.moveRobots(100);
		return bathroomSecurity.safetyFactor();
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.size() < 100)
			return -1; // Part two doesn't work on sample input

		auto bathroomSecurity = BathroomSecurity{input, 101, 103};
		int seconds = 0;
		while (!bathroomSecurity.maybeTree())
		{
			++seconds;
			std::cout << seconds << "\n";
			bathroomSecurity.moveRobots();
		}

		bathroomSecurity.drawRobots();

		return seconds;
	}
};
