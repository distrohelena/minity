#pragma once
#include "AssetObject.h"
#include "structs.h"

class AssetTexture;
class AssetMaterial : public AssetObject
{
public:
	float4 color;
	AssetTexture* texture;

	float4* uvs;
};

