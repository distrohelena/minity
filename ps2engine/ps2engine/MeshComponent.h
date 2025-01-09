#pragma once
#include "GameComponent.h"
#include "AssetMesh.h"

class AssetMaterial;
class MeshComponent : public GameComponent
{
public:
	AssetMesh* mesh;
	AssetMaterial* mat;

	void OnEnable(void) override;

};

