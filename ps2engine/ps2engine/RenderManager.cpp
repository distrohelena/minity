#include "RenderManager.h"

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

void RenderManager::RegisterLight(LightComponent* light) {
	lights.push_back(light);
}

void RenderManager::LoadMesh(AssetMesh* mesh, int totalVerts, float4* vertices, float4* normals, float4* uvs) {
	
}

void RenderManager::LoadTexture(AssetTexture* tex, unsigned int width, unsigned int height, uint8_t pixelWidth) {

}

void RenderManager::BeginRenderFrame() {

}

void RenderManager::EndRenderFrame() {

}
