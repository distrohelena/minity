#include "RenderManager.h"
AssetMesh* RenderManager::LoadMesh(int count, float4* vertices, float4* normals, float4* uvs) {
	return nullptr;
}

void RenderManager::InitPipeline() {
}

void RenderManager::Render() {
}

void RenderManager::RegisterMesh(MeshComponent* mesh) {
	meshesToDraw.push_back(mesh);
}

void RenderManager::RegisterCamera(CameraComponent* camera) {
	cameras.push_back(camera);
}