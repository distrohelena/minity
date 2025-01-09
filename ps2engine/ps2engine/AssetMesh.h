#pragma once

#include "AssetObject.h"

#if DIRECTX
#include <d3d11.h>
#endif

#ifdef PS2
#include <tyra>
using namespace Tyra;
#endif

class AssetMesh : public AssetObject
{
public: 
	int vertexCount;

#if DIRECTX
	ID3D11Buffer* vertexBuffer;
#endif

#ifdef PS2
	std::unique_ptr<StaticMesh> mesh;
#endif

};

