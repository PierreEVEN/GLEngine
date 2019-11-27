#include "../Asset/asset.h"


class WorldAsset : public Asset
{

public:
	WorldAsset(std::string filePath);
	virtual void Parse(const Document& data) override;
};