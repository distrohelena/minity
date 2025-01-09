#pragma once

#include "GraphicsDevice.h"
#include "RenderManager.h"
#include "SceneManager.h"

class Core
{
public:
	Core();
	static Core* instance;

	void Init(HWND hWnd);
	void Draw(void);

	SceneManager* GetSceneManager() {
		return sceneManager;
	}

	RenderManager* GetRenderManager() {
		return renderManager;
	}

private:
	ContentManager* contentManager;
	SceneManager* sceneManager;
	GraphicsDevice* device;
	RenderManager* renderManager;
};

