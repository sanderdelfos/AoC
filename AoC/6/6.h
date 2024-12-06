#pragma once
#include <execution>
#include <map>

#include "Input.h"

enum class Direction
{
	Up,
	Right,
	Down,
	Left,
};

class Map : public Grid
{
public:
	Map(std::vector<std::string> const& input, bool convertToInt)
		: Grid(input, convertToInt)
	{
		for (auto& line : data)
			for (auto& coordinate : line)
				if (coordinate.strValue == "^")
					current = &coordinate;
	}

	static Direction turn(Direction direction)
	{
		switch (direction)
		{
		case Direction::Up:
			return Direction::Right;
		case Direction::Right:
			return Direction::Down;
		case Direction::Down:
			return Direction::Left;
		case Direction::Left:
			return Direction::Up;
		}
		throw std::runtime_error("");
	}

	static std::string drawDirection(Direction direction)
	{
		switch (direction)
		{
		case Direction::Up:
		case Direction::Down:
			return "|";
		case Direction::Left:
		case Direction::Right:
			return "-";
		}
		throw std::runtime_error("");
	}

	Coordinate& step()
	{
		const auto [dy, dx] = getStep();
		if (boundsCheck(*current, dy, dx))
			return get(*current, dy, dx);
		return *current;
	}

	std::pair<int, int> getStep() const
	{
		switch (direction)
		{
		case Direction::Up:
			return {-1, 0};
		case Direction::Right:
			return {0, 1};
		case Direction::Down:
			return {1, 0};
		case Direction::Left:
			return {0, -1};
		}
		throw std::runtime_error("");
	}

	// return if loop
	bool walk()
	{
		while (true)
		{
			if (visited[current->location].contains(direction))
				return true; // loop

			visited[current->location].insert(direction);
			current->strValue = drawDirection(direction);
			auto& next = step();
			if (next.strValue == "#")
				direction = turn(direction);
			else if (&next == current)
				return false;
			else
				current = &next;
		}
	}

	int countVisited() const
	{
		return static_cast<int>(visited.size());
	}

	Coordinate* current;
	Direction direction{Direction::Up};
	std::map<Location, std::set<Direction>> visited;
};


class Six : public Puzzle
{
public:
	explicit Six()
		: Puzzle(6)
	{
	}

	int partOne(const Input& input) override
	{
		Map map{input.lines, false};
		map.walk();
		//map.draw();

		return map.countVisited();
	}

	int partTwo(const Input& input) override
	{
		Map const map{input.lines, false};
		std::atomic_int count = 0;
		for (auto const& line : map.data)
		{
			std::for_each(std::execution::par_unseq, line.begin(), line.end(), [&](Coordinate const& coordinate)
			{
				auto copy = map;
				auto& editCoordinate = copy.get(coordinate.location);
				if (editCoordinate.strValue == "#")
					return;

				editCoordinate.strValue = "#";
				if (copy.walk())
					++count;
				//map.draw();
				//std::cout << coordinate.y() << ", " << coordinate.x() << "\n";
			});
		}

		return count;
	}
};
