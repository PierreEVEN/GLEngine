#pragma once

#include <string>

class Asset;
class World;

struct AssetSelector
{
public:

	std::string label;

	void Draw(World* inWorld);
	Asset* selectedAsset = nullptr;
};