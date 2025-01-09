#include "Core.h"
#include "Buffer.hpp"
#include "structs.h"

#if DIRECTX
#include "DirectXRenderer.h";
#endif

Core* Core::instance = nullptr;

Core::Core() {
	instance = this;
}

void Core::Init(HWND hWnd) {
	device = new GraphicsDevice();
	device->InitD3D(hWnd);

#if DIRECTX
	renderManager = new DirectXRenderer(device);
#else 
	throw;
#endif

	renderManager->InitPipeline();

	contentManager = new ContentManager();

	sceneManager = new SceneManager(renderManager, contentManager);
	sceneManager->ParseAssets("assets.ps2");
	sceneManager->ParseScene("scene.ps2");
	
	int x = -1;
}

void Core::Draw() {
	device->BeginRenderFrame();
	renderManager->Render();
	device->EndRenderFrame();
}