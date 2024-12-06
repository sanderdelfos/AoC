#pragma once
#include "Input.h"

class Two : public Puzzle
{
public:
	explicit Two()
		: Puzzle("two")
	{
	}

	int partOne(const Input& input) override
	{
		return -1;
	}

	int partTwo(const Input& input) override
	{
		return -1;
	}
};
