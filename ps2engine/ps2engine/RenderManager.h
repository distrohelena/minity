#pragma once
#include "structs.h"
#include "AssetMesh.h"
#include <vector>

using namespace std;

class MeshComponent;
class CameraComponent;
class RenderManager
{
public:
	virtual void InitPipeline(void);
	virtual AssetMesh* LoadMesh(int count, float4* vertices, float4* normals, float4* uvs);
	virtual void Render(void);

	virtual void RegisterMesh(MeshComponent* mesh);
	virtual void RegisterCamera(CameraComponent* cam);

	vector<MeshComponent*> meshesToDraw;
	vector<CameraComponent*> cameras;
};

