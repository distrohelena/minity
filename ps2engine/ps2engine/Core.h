#pragma once

#include "RenderManager.h"
#include "SceneManager.h"
#include "InputManager.h"

#ifdef PS2
#include <tyra>
using namespace Tyra;
#endif

class Core {
 public:
  Core();
  static Core* instance;

#ifdef DIRECTX
  void Init(HWND hWnd);

  HWND GetHWND() { return hWND; }
#endif

#ifdef PS2
  void Init(Engine* engine);

  Engine* GetEngine() { return engine; }
#endif

  void Draw(void);

  SceneManager* GetSceneManager() { return sceneManager; }

  RenderManager* GetRenderManager() { return renderManager; }

  InputManager* GetInputManager() { return inputManager; }

 private:
  ContentManager* contentManager;
  SceneManager* sceneManager;
  RenderManager* renderManager;
  InputManager* inputManager;

#if DIRECTX
  HWND hWND;
#endif

#ifdef PS2
  Engine* engine;
#endif
};
