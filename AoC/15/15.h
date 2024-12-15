#pragma once
#include "Input.h"

class Warehouse : public Grid
{
public:
	static auto constexpr WallStr = "#";
	static auto constexpr SmallBoxStr = "O";
	static auto constexpr LeftBoxStr = "[";
	static auto constexpr EmptyStr = ".";
	static auto constexpr RobotStr = "@";

	Warehouse(const Input& input) : Grid(getGridInput(input), false)
	{
		parseRobotMoves(input);
		robot = getRobot();
	}

	static std::string expandChar(auto ch)
	{
		switch (ch)
		{
		case WallStr[0]:
			return "##";
		case SmallBoxStr[0]:
			return "[]";
		case EmptyStr[0]:
			return "..";
		case RobotStr[0]:
			return "@.";
		}
		return {ch};
	}

	static Input expandInput(Input const& input)
	{
		Input expanded{input};
		expanded.lines.clear();
		for (auto const& line : input.lines)
		{
			auto& expanedLine = expanded.lines.emplace_back();
			for (auto const ch : line)
				expanedLine += expandChar(ch);
		}
		return expanded;
	}

	static std::vector<std::string> getGridInput(const Input& input)
	{
		std::vector<std::string> gridlines;
		for (auto const& line : input.lines)
		{
			if (line.size() < 2)
				return gridlines;

			gridlines.push_back(line);
		}
		return gridlines;
	}

	static std::optional<Location> getLocationFromMove(auto ch)
	{
		switch (ch)
		{
		case '<':
			return Location{0, -1};
		case '>':
			return Location{0, 1};
		case '^':
			return Location{-1, 0};
		case 'v':
			return Location{1, 0};
		}
		return std::nullopt;
	}

	void parseRobotMoves(const Input& input)
	{
		for (auto const& line : input.lines)
		{
			if (line.contains(WallStr[0]))
				continue;

			for (auto const ch : line)
				if (auto location = getLocationFromMove(ch))
					robotMoves.push_back(*location);
		}
	}

	Location getRobot() const
	{
		for (auto const& line : data)
			for (auto const& coordinate : line)
				if (coordinate.strValue == RobotStr)
					return coordinate.location;
		return {};
	}

	bool isWall(Location const& location)
	{
		return get(location).strValue == WallStr;
	}

	bool isEmpty(Location const& location)
	{
		return get(location).strValue == EmptyStr;
	}

	bool isSmallBox(Location const& location)
	{
		return get(location).strValue == SmallBoxStr;
	}

	void moveBoxes(Location const& move, Location const& newRobotLocation)
	{
		auto location = newRobotLocation;
		while (true)
		{
			location = location.move(move);
			if (isWall(location))
				return; // boxes can't be moved, wall in the way!
			if (isEmpty(location))
				break; // free space behind the boxes, lets move them!
		}

		get(newRobotLocation).strValue = RobotStr;
		get(location).strValue = SmallBoxStr;
		get(robot).strValue = EmptyStr;
		robot = newRobotLocation;
	}

	Location getOtherBoxPart(Location const& boxPart)
	{
		return get(boxPart).strValue == LeftBoxStr ? boxPart.move({0, 1}) : boxPart.move({0, -1});
	}

	void addLocation(std::map<Location, std::string>& map, Location const& location)
	{
		map[location] = get(location).strValue;
	}

	void moveWideBoxes(Location const& move, Location const& newRobotLocation)
	{
		// newRobotLocation is also the location of the box that the robot is trying to push

		std::map<Location, std::string> boxes;
		addLocation(boxes, newRobotLocation);
		addLocation(boxes, getOtherBoxPart(newRobotLocation));

		while (true)
		{
			std::set<Location> adjacentBoxes;
			auto allAdjacentFound = true;
			for (auto const& boxPartLocation : boxes | std::views::keys)
			{
				auto const location = boxPartLocation.move(move);
				if (boxes.contains(location))
					continue;

				if (isWall(location))
					return;
				if (isEmpty(location))
					continue;

				allAdjacentFound = false;
				adjacentBoxes.insert(location);
				adjacentBoxes.insert(getOtherBoxPart(location));
			}
			for (auto const& adjacentBox : adjacentBoxes)
				addLocation(boxes, adjacentBox);

			if (allAdjacentFound)
				break;
		}
		for (const auto& boxLocation : boxes | std::views::keys)
			get(boxLocation).strValue = EmptyStr;

		for (auto const& [boxLocation, boxValue] : boxes)
			get(boxLocation.move(move)).strValue = boxValue;

		get(newRobotLocation).strValue = RobotStr;
		get(robot).strValue = EmptyStr;
		robot = newRobotLocation;
	}

	void moveRobot(Location const& move)
	{
		auto const newRobotLocation = robot.move(move);
		auto& newRobotLocationValue = get(newRobotLocation).strValue;
		if (newRobotLocationValue == WallStr)
			return;

		if (newRobotLocationValue == EmptyStr)
		{
			newRobotLocationValue = RobotStr;
			get(robot).strValue = EmptyStr;
			robot = newRobotLocation;
			return;
		}

		if (newRobotLocationValue == SmallBoxStr)
			return moveBoxes(move, newRobotLocation);

		moveWideBoxes(move, newRobotLocation);
	}

	void moveRobot()
	{
		for (auto const& move : robotMoves)
		{
			moveRobot(move);
			draw();
		}
	}

	static long long getGPS(Location const& location)
	{
		return (100 * location.y()) + location.x();
	}

	long long sumBoxesGPS() const
	{
		auto sum = 0ll;
		for (auto const& line : data)
			for (auto const& coordinate : line)
				if (coordinate.strValue == SmallBoxStr || coordinate.strValue == LeftBoxStr)
					sum += getGPS(coordinate.location);
		return sum;
	}

	Location robot;
	std::vector<Location> robotMoves;
};

class Fifteen : public Puzzle
{
public:
	explicit Fifteen()
		: Puzzle(15)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto warehouse = Warehouse{input};
		warehouse.moveRobot();
		warehouse.draw();
		return warehouse.sumBoxesGPS();
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto warehouse = Warehouse{Warehouse::expandInput(input)};
		warehouse.moveRobot();
		warehouse.draw();
		return warehouse.sumBoxesGPS();
	}
};
