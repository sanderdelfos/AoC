#pragma once
#include "Input.h"

class Four : public Puzzle
{
public:
	explicit Four()
		: Puzzle(4)
	{
	}

	static void draw(Grid const& grid, int const count)
	{
		std::cout << "\n";
		for (auto const& line : grid.data)
		{
			for (auto const& coordinate : line)
			{
				std::cout << coordinate.strValue;
			}
			std::cout << "\n";
		}
		std::cout << count << " times XMAS found\n";
	}

	static int countHorizontal(Grid const& grid)
	{
		int count = 0;
		for (auto const& line : grid.data)
		{
			std::string xmas = "XMAS";
			int xmasIndex = 0;
			int const xmasEnd = xmas.size();
			for (auto const& coordinate : line)
			{
				if (coordinate.strValue == std::string{xmas[xmasIndex]})
					++xmasIndex;
				else if (coordinate.strValue == std::string{xmas[0]})
					xmasIndex = 1;
				else
					xmasIndex = 0;

				if (xmasIndex == xmasEnd)
				{
					++count;
					xmasIndex = 0;
				}
			}
		}
		//draw(grid, count);
		return count;
	}

	static int countDiagonal(Grid const& grid)
	{
		int count = 0;
		int const xMax = grid.xMax();
		int const yMax = grid.yMax();
		const std::string xmas = "XMAS";
		int const xmasEnd = xmas.size();

		for (auto const y : range(yMax))
		{
			for (auto const x : range(xMax))
			{
				int xmasIndex = 0;
				int xCur = x, yCur = y;
				while (yCur < yMax && xCur < xMax)
				{
					auto const& coordinate = grid.data[yCur][xCur];
					if (coordinate.strValue == std::string{xmas[xmasIndex]})
						++xmasIndex;
					else
						break;

					if (xmasIndex == xmasEnd)
					{
						++count;
						break; 
					}

					++yCur;
					++xCur;
				}
			}
		}

		//draw(grid, count);
		return count;
	}

	static bool isMas(Grid& grid, Coordinate const& coordinate, bool const flip)
	{
		auto const d = flip ? -1 : 1;
		return coordinate.strValue == "M" && grid.get(coordinate, d, 1).strValue == "A" && grid.get(
				coordinate, 2 * d, 2).
			strValue == "S";
	}

	static bool isSam(Grid& grid, Coordinate const& coordinate, bool const flip)
	{
		auto const d = flip ? -1 : 1;
		return coordinate.strValue == "S" && grid.get(coordinate, d, 1).strValue == "A" && grid.get(
				coordinate, 2 * d, 2).
			strValue == "M";
	}

	static bool isSamOrMas(Grid& grid, Coordinate const& coordinate, bool const flip)
	{
		return isMas(grid, coordinate, flip) || isSam(grid, coordinate, flip);
	}

	static int countXMas(Grid& grid)
	{
		int count = 0;
		int const xMax = grid.xMax();
		int const yMax = grid.yMax();

		for (auto const y : range(yMax - 2))
		{
			for (auto const x : range(xMax - 2))
			{
				auto const& first = grid.get(y, x);
				auto const& second = grid.get(y + 2, x);
				if (isSamOrMas(grid, first, false) && isSamOrMas(grid, second, true))
					++count;
			}
		}

		return count;
	}


	long long partOne(const Input& input) override
	{
		auto const grid = Grid{input.lines, false};
		auto const horizontal = countHorizontal(grid);
		auto const horizontalMirrored = countHorizontal(grid.mirrorVertical());
		auto const vertical = countHorizontal(grid.transpose());
		auto const verticalMirrored = countHorizontal(grid.transpose().mirrorVertical());
		auto const diagonal = countDiagonal(grid); // 1
		auto const diagonalMirroredVertical = countDiagonal(grid.mirrorVertical()); // 1
		auto const diagonalMirroredHorizontal = countDiagonal(grid.mirrorHorizontal());
		auto const diagonalMirroredHorizontalVertical = countDiagonal(grid.mirrorHorizontal().mirrorVertical());

		return horizontal + horizontalMirrored + vertical + verticalMirrored + diagonal + diagonalMirroredVertical +
			diagonalMirroredHorizontal + diagonalMirroredHorizontalVertical;
	}

	long long partTwo(const Input& input) override
	{
		auto grid = Grid{input.lines, false};
		return countXMas(grid);
	}
};
