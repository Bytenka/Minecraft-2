#include "Block.h"

namespace tk
{
namespace BlockDatas
{
const BlockData opaque(true);
const BlockData transparent(false);
} // namespace BlockDatas

namespace Blocks
{
const Block _air("_air", BlockDatas::transparent);
const Block _opaque("_opaque", BlockDatas::opaque);
const Block grass("grass", "grass_top", "dirt", "grass_side", "grass_side", "grass_side", "grass_side", BlockDatas::opaque);
const Block dirt("dirt", BlockDatas::opaque);
const Block cobble_stone("cobble_stone", BlockDatas::opaque);
const Block stone("stone", BlockDatas::opaque);
const Block obsidian("obsidian", BlockDatas::opaque);
const Block tnt("tnt", "tnt_top", "tnt_bottom", "tnt_side", "tnt_side", "tnt_side", "tnt_side", BlockDatas::opaque);
const Block diamond_ore("diamond_ore", BlockDatas::opaque);
const Block bedrock("bedrock", BlockDatas::opaque);
} // namespace Blocks

} // namespace tk