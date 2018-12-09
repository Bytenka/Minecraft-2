#pragma once

#include <glad/glad.h>

#include <string>
#include <array>

namespace tk
{
struct BlockData
{
	BlockData(bool isOpaque)
		: isOpaque(isOpaque) {}

	bool isOpaque;
};

struct Block
{
	Block(const std::string &block_name, const BlockData &data)
		: block_name(block_name), data(data)
	{
		for (unsigned i = 0; i < 6; i++)
		{
			textures[i] = block_name;
		}
	}

	Block(
		const std::string &block_name,
		const std::string &texture_top,
		const std::string &texture_bottom,
		const std::string &texture_left,
		const std::string &texture_right,
		const std::string &texture_front,
		const std::string &texture_back,
		const BlockData &data) : block_name(block_name), data(data)
	{
		textures[0] = texture_top;
		textures[1] = texture_bottom;
		textures[2] = texture_left;
		textures[3] = texture_right;
		textures[4] = texture_front;
		textures[5] = texture_back;
	}

	std::string block_name;
	std::array<std::string, 6> textures;
	const BlockData &data;

	//inline bool operator==(const Block& rhs) { return this == &rhs; }
	//inline bool operator==(const Block& rhs) { return block_name == rhs.block_name; }
};

namespace BlockDatas
{
extern const BlockData transparent;
extern const BlockData opaque;
} // namespace BlockDatas

namespace Blocks
{
extern const Block _air;
extern const Block _opaque;
extern const Block grass;
extern const Block dirt;
extern const Block cobble_stone;
extern const Block stone;
extern const Block obsidian;
extern const Block tnt;
extern const Block diamond_ore;
extern const Block bedrock;
} // namespace Blocks

enum BlockFace
{
	TOP = 0,
	BOTTOM,
	LEFT,
	RIGHT,
	FRONT,
	BACK
};

// Access these using the enum above
extern const std::array<std::array<GLfloat, 12>, 6> blockFaceCoordinates;
} // namespace tk