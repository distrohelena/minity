#pragma once
#include "AssetMesh.h"
#include <vector>
#include "float3.h"
#include "float4.h"

using namespace std;

class MeshComponent;
class CameraComponent;
class LightComponent;
class AssetTexture;

class RenderManager
{
public:
	virtual void InitPipeline(void);
	virtual void LoadMesh(AssetMesh* mesh, int totalVerts, float4* vertices, float4* normals, float4* uvs);
	virtual void LoadTexture(AssetTexture* tex, unsigned int width, unsigned int height, uint8_t pixelWidth);
	virtual void Render(void);

	virtual void RegisterMesh(MeshComponent* mesh);
	virtual void RegisterCamera(CameraComponent* cam);
	virtual void RegisterLight(LightComponent* light);

	virtual void BeginRenderFrame(void);
	virtual void EndRenderFrame(void);

	vector<MeshComponent*> meshesToDraw;
	vector<CameraComponent*> cameras;
	vector<LightComponent*> lights;
};

