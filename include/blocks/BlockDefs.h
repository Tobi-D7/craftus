#pragma once

#include <Block.h>

Block BlockDef_Air = Block_new(Block_Air);
Block BlockDef_Stone = Block_new(Block_Stone);
Block BlockDef_Dirt = Block_new(Block_Dirt);
Block BlockDef_Grass = Block_new(Block_Grass);
Block BlockDef_Cobblestone = Block_new(Block_Cobblestone);
Block BlockDef_Sand = Block_new(Block_Sand);
Block BlockDef_Log = Block_new(Block_Log);
Block BlockDef_Gravel = Block_new(Block_Gravel);
Block BlockDef_Leaves = Block_new(Block_Leaves);
Block BlockDef_Glass = Block_new(Block_Glass);
Block BlockDef_Stonebrick = Block_new(Block_Stonebrick);
Block BlockDef_Brick = Block_new(Block_Brick);
Block BlockDef_Planks = Block_new(Block_Planks);
Block BlockDef_Wool = Block_new(Block_Wool);
Block BlockDef_Bedrock = Block_new(Block_Bedrock);
Block BlockDef_Coarse = Block_new(Block_Coarse);
Block BlockDef_Door_Top = Block_new(Block_Door_Top);
Block BlockDef_Door_Bottom = Block_new(Block_Door_Bottom);
Block BlockDef_Snow = Block_new(Block_Snow_Grass);
Block BlockDef_Snow = Block_new(Block_Snow);
Block BlockDef_Obsidian = Block_new(Block_Obsidian);
Block BlockDef_Netherrack = Block_new(Block_Netherrack);
Block BlockDef_Sandstone = Block_new(Block_Sandstone);
Block BlockDef_Smooth = Block_new(Block_Smooth_Stone);
Block BlockDef_Crafting_Table = Block_new(Block_Crafting_Table);
Block BlockDef_Grass = Block_new(Block_Grass_Path);
Block BlockDef_Water = Block_new(Block_Water);
Block BlockDef_Lava = Block_new(Block_Lava);
Block BlockDef_Iron_Ore = Block_new(Block_Iron_Ore);
Block BlockDef_Coal_Ore = Block_new(Block_Coal_Ore);
Block BlockDef_Diamond_Ore = Block_new(Block_Diamond_Ore);
Block BlockDef_Gold_Ore = Block_new(Block_Gold_Ore);
Block BlockDef_Emerald_Ore = Block_new(Block_Emerald_Ore);
Block BlockDef_Gold_Block = Block_new(Block_Gold_Block);
Block BlockDef_Diamond_Block = Block_new(Block_Diamond_Block);
Block BlockDef_Coal_Block = Block_new(Block_Coal_Block);
Block BlockDef_Iron_Block = Block_new(Block_Iron_Block);
Block BlockDef_Emerald_Block = Block_new(Block_Emerald_Block);
Block BlockDef_Furnace = Block_new(Block_Furnace);
Block BlockDef_Count = Block_new(Blocks_Count);

Process finished with exit code 0


Block Block_new(int def_id, char def_liquidType = 0) {
    Block block = Block();
    block.id = def_id;
    block.liquidType = def_liquidType;
    return block;
}