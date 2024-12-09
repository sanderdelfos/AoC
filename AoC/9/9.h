#pragma once
#include "Input.h"

struct Block
{
	bool empty;
	int id;
};

struct File
{
	int id;
	int startIndex;
	int endIndex;
	int size;
};

class Disk
{
public:
	Disk(std::string const& input)
	{
		int id = 0;
		bool freeSpace = false;
		int index = 0;
		for (auto const& ch : input)
		{
			auto const size = std::stoi(std::string{ch});
			if (!freeSpace)
			{
				auto const block = Block{false, id};
				blocks.insert_range(blocks.end(), std::vector<Block>(size, block));

				auto const file = File{id, index, index + size, size};
				files.push_back(file);

				++id;
			}
			else
			{
				constexpr auto block = Block{true, -1};
				blocks.insert_range(blocks.end(), std::vector<Block>(size, block));

				auto const file = File{id, index, index + size, size};
				spaces.push_back(file);
			}

			freeSpace = !freeSpace;
			index += size;
		}
	}

	auto& getLastFile()
	{
		for (auto& block : blocks | std::views::reverse)
			if (!block.empty)
				return block;
		throw std::runtime_error("");
	}

	auto& getFirstEmptyBlock()
	{
		for (auto& block : blocks)
			if (block.empty)
				return block;
		throw std::runtime_error("");
	}

	void moveOne()
	{
		auto& lastfile = getLastFile();
		auto& firstEmptyBlock = getFirstEmptyBlock();

		lastfile.empty = true;
		firstEmptyBlock.id = lastfile.id;
		firstEmptyBlock.empty = false;
	}

	void moveFiles()
	{
		auto nonMovedFiles = files;
		auto nonFilledSpaces = spaces;
		while (!nonMovedFiles.empty())
		{
			auto& lastNonMovedFile = nonMovedFiles.back();
			for (auto& nonFilledSpace : nonFilledSpaces)
			{
				if (nonFilledSpace.startIndex > lastNonMovedFile.startIndex)
					break; // no need to look any further

				if (nonFilledSpace.size >= lastNonMovedFile.size)
				{
					for (auto index : range(nonFilledSpace.startIndex,
					                        nonFilledSpace.startIndex + lastNonMovedFile.size))
					{
						auto& block = blocks[index];
						block.id = lastNonMovedFile.id;
						block.empty = false;
					}

					nonFilledSpace.size -= lastNonMovedFile.size;
					nonFilledSpace.startIndex += lastNonMovedFile.size;

					for (auto const& index : range(lastNonMovedFile.startIndex, lastNonMovedFile.endIndex))
					{
						auto& block = blocks[index];
						block.empty = true;
					}

					break;
				}
			}
			nonMovedFiles.pop_back();
		}
	}

	void move()
	{
		while (!allMoved())
			moveOne();
	}

	bool allMoved() const
	{
		auto emptyFound = false;
		for (auto const& block : blocks)
		{
			if (emptyFound && !block.empty)
				return false;

			if (block.empty)
				emptyFound = true;
		}
		return true;
	}

	long long checksum() const
	{
		auto checksum = 0ll;
		for (auto const index : range(blocks.size()))
		{
			auto const& block = blocks.at(index);
			if (!block.empty)
				checksum += (block.id * index);
		}
		return checksum;
	}

	void draw() const
	{
		std::cout << "\n\n";
		for (auto const& block : blocks)
		{
			if (block.empty)
				std::cout << ".";
			else
				std::cout << block.id;
		}
		std::cout << "\n\n";
	}

	std::vector<Block> blocks;
	std::vector<File> files;
	std::vector<File> spaces;
};


class Nine : public Puzzle
{
public:
	explicit Nine()
		: Puzzle(9)
	{
	}

	long long partOne(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto& line = input.lines[0];
		auto disk = Disk(line);
		//disk.draw();
		disk.move();
		//disk.draw();
		return disk.checksum();
	}

	long long partTwo(const Input& input) override
	{
		if (input.lines.empty())
			return -1;

		auto& line = input.lines[0];
		auto disk = Disk(line);
		//disk.draw();
		disk.moveFiles();
		//disk.draw();
		return disk.checksum();
	}
};
