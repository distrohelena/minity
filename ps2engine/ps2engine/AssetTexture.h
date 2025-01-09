#pragma once
#include "AssetObject.h"
#include "structs.h"

#ifdef DIRECTX
#include <d3d11.h>
#endif

#ifdef PS2
#include <tyra>
using namespace Tyra;
#endif

class AssetTexture : public AssetObject {
public:
	~AssetTexture();
	unsigned char* data;

#ifdef DIRECTX
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
#endif

#ifdef PS2
	Texture* texture = nullptr;
#endif
};
