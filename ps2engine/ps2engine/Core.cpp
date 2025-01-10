#include "Core.h"

#ifdef DIRECTX
#include "DirectXRenderer.h"
#include "DirectInputManager.h"
#endif

#ifdef PS2
#include "TyraRenderManager.h"
#include "TyraInputManager.h"
#endif

Core* Core::instance = nullptr;

Core::Core() { instance = this; }

#if DIRECTX
void Core::Init(HWND hWnd) {
  hWND = hWnd;
  renderManager = new DirectXRenderer(hWnd);
  renderManager->InitPipeline();

  inputManager = new DirectInputManager();
  inputManager->Init();

  contentManager = new ContentManager();

  sceneManager = new SceneManager(renderManager, contentManager);
  sceneManager->ParseAssets("assets.ps2");
  sceneManager->ParseScene("scene.ps2");
}
#endif

#ifdef PS2
void Core::Init(Engine* eng) {
  engine = eng;

  renderManager = new TyraRenderManager(engine);
  renderManager->InitPipeline();

  inputManager = new TyraInputManager();
  inputManager->Init();

  contentManager = new ContentManager();

  sceneManager = new SceneManager(renderManager, contentManager);
  sceneManager->ParseAssets("assets.ps2");
  sceneManager->ParseScene("scene.ps2");
  // sceneManager->ParseAssets("mass://assets.ps2");
  // sceneManager->ParseScene("mass://scene.ps2");
}
#endif

void Core::Draw() {
  inputManager->Tick();

  for (GameObject* obj : sceneManager->rootObjs) {
    obj->Tick();
  }

  renderManager->BeginRenderFrame();
  renderManager->Render();
  renderManager->EndRenderFrame();
}