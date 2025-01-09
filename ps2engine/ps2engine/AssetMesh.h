#pragma once
#include <d3d11.h>
#include "AssetObject.h"

class AssetMesh : public AssetObject
{
public:
#if DIRECTX
	ID3D11Buffer* pVBuffer;    // global
	int vertexCount;
#endif

};

