#pragma once
#include <iostream>
#include <fstream>
#include "structs.h"
#include "AssetMesh.h"
#include "RenderManager.h"
#include "ContentManager.h"
#include "GameObject.h"
#include <vector>

using namespace std;

class SceneManager
{
public:
	SceneManager(RenderManager* renderer, ContentManager* content);

	void ParseScene(const char* sceneName);
	void ParseAssets(const char* assetsName);

private:
	vector<GameObject*> rootObjs;

	RenderManager* renderer;
	ContentManager* content;

	GameObject* ReadGameObject(ifstream& stream);
};

