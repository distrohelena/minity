#pragma once
#include "GameComponent.h"
#include "AssetMesh.h"

class MeshComponent : public GameComponent
{
public:
	AssetMesh* mesh;

	void OnEnabled(void) override;

};

