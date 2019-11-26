#ifndef ASSET_H
#define ASSET_H

#include "rapidjson/document.h"

using namespace rapidjson;

class Asset
{

private:

	std::string assetName;
	std::string assetPath;

public:

	Asset();
	Asset(std::string inAssetPath);

	virtual void Parse(const Document& data);
	virtual void Serialize();

	void LoadAsset();

	std::string GetName() const { return assetName; }
};
#endif