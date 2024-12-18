#pragma once
#include <execution>

#include "Input.h"


class Game
{
public:
	static auto constexpr aTokens = 3;
	static auto constexpr bTokens = 1;
	static auto constexpr correction = 10000000000000ll;

	Game(std::vector<std::string> const& input, bool withCorrection)
	{
		dA = parse(input[0]);
		dB = parse(input[1]);
		prize = parse(input[2]);
		if (withCorrection)
		{
			prize.first += correction;
			prize.second += correction;
		}
	}

	static int parsePart(const std::string& part)
	{
		return std::stoi(part.substr(2));
	}

	static Location parse(std::string line)
	{
		line = split(line, ": ")[1];
		auto const parts = split(line, ", ");
		return Location{parsePart(parts[1]), parsePart(parts[0])};
	}

	double distancePerTokenA() const
	{
		return static_cast<double>(dA.getManhattenDistance({0, 0})) / aTokens;
	}

	double distancePerTokenB() const
	{
		return static_cast<double>(dB.getManhattenDistance({0, 0})) / bTokens;
	}

	Location play(long long a, long long b) const
	{
		auto const dy = a * dA.y() + b * dB.y();
		auto const dx = a * dA.x() + b * dB.x();
		return Location{dy, dx};
	}

	long long findMaxPushes(Location& location, const Location& d) const
	{
		auto const dx = std::abs(location.x() - prize.x());
		auto const dy = std::abs(location.y() - prize.y());

		auto const maxPushesX = dx / d.x();
		auto const maxPushesY = dy / d.y();

		auto const maxPushes = min(maxPushesX, maxPushesY);
		auto const dxMax = maxPushes * d.x();
		auto const dyMax = maxPushes * d.y();
		location = location.move({dyMax, dxMax});
		return maxPushes;
	}

	void iterate(Location const& largerD, long long& largerNumPushes, Location const& smallerD,
	             long long& smallerNumPushes) const
	{
		Location location{0, 0};
		largerNumPushes = findMaxPushes(location, largerD);
		while (location != prize && largerNumPushes >= 0)
		{
			auto dSmallerNumPushes = findMaxPushes(location, smallerD);
			while (dSmallerNumPushes == 0)
			{
				location = location.moveBack(largerD);
				--largerNumPushes;
				dSmallerNumPushes = findMaxPushes(location, smallerD);
			}
			smallerNumPushes += dSmallerNumPushes;
		}
	}

	long long play() const
	{
		auto numPushesB = 0ll;
		auto numPushesA = 0ll;

		if (distancePerTokenA() > distancePerTokenB())
			iterate(dA, numPushesA, dB, numPushesB);
		else
			iterate(dB, numPushesB, dA, numPushesA);

		std::cout << std::format("\ngame finished! {} {}", numPushesA, numPushesB);

		if (numPushesA < 0 || numPushesB < 0)
			return 0; // no solution

		return 3ll * numPushesA + numPushesB;
	}

	long long playSmart() const
	{
		// This obviously isn't really the smartest way. You can probably solve this with linear algebra. But I don't like algebra!
		// Note that there is only one solution (in any), so we can just decide what direction we need to go and what move helps us most,
		// and multiply that move by a value that we are quite certain won't overshoot.  

		auto numPushesB = 0ll;
		auto numPushesA = 0ll;

		Location location{0, 0};
		auto const aDirection = location.getDirection(dA);
		auto const bDirection = location.getDirection(dB);

		while (location.x() < prize.x() && location.y() < prize.y())
		{
			auto const distance = prize.getManhattenDistance(location);
			auto move = [&](Location const& d, long long& pushes)
			{
				auto const multiplier = max(1ll, (distance / d.getManhattenDistance({0, 0})) / 10);
				location = location.move(d, multiplier);
				pushes += multiplier;
			};

			auto const prizeDirection = location.getDirection(prize);
			auto const dADirection = std::abs(prizeDirection - aDirection);
			auto const dBDirection = std::abs(prizeDirection - bDirection);

			if (dADirection < dBDirection)
				move(dA, numPushesA);
			else
				move(dB, numPushesB);
		}


		if (location != prize)
		{
			std::cout << std::format("\nNo prize! {} {}\n", numPushesA, numPushesB);
			return 0; // No prize!! :(
		}

		std::cout << std::format("\nWin! {} {}\n", numPushesA, numPushesB);
		return 3 * numPushesA + numPushesB;
	}

	Location dA;
	Location dB;
	Location prize;
};

class Games
{
public:
	Games(const Input& input, bool withCorrection)
	{
		std::vector<std::string> buffer;
		for (auto const i : range(input.lines.size()))
		{
			if (i > 0 && (i + 1) % 4 == 0)
			{
				list.emplace_back(buffer, withCorrection);
				buffer.clear();
			}
			else
			{
				buffer.push_back(input.lines[i]);
			}
		}
		list.emplace_back(buffer, withCorrection);
	}

	long long play() const
	{
		auto count = 0ll;
		for (auto const& game : list)
		{
			count += game.playSmart();
		}
		return count;
	}

	std::vector<Game> list;
};

class Thirteen : public Puzzle
{
public:
	explicit Thirteen()
		: Puzzle(13)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto games = Games{input, false};
		return games.play();
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto games = Games{input, true};
		return games.play();
	}
};
