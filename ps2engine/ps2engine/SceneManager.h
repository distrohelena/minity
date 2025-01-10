#pragma once
#include <iostream>
#include <fstream>
#include "AssetMesh.h"
#include "RenderManager.h"
#include "ContentManager.h"
#include "GameObject.h"
#include <vector>
#include "ComponentType.h"
#include "MeshComponent.h"
#include "CameraComponent.h"
#include <cstdint>
#include "LightmapMode.h"

using namespace std;

class AssetTexture;
class SceneManager
{
public:
	SceneManager(RenderManager* renderer, ContentManager* content);

	void ParseScene(const char* sceneName);
	void ParseAssets(const char* assetsName);
	vector<GameObject*> rootObjs;
	vector<AssetTexture*> lightmaps;

private:
	void parseTexture(ifstream& stream, char guid[33], bool isLightmap);

	unsigned int counter = 0;
  	LightmapMode lightmap;

	RenderManager* renderer;
	ContentManager* content;

	GameObject* ReadGameObject(ifstream& stream);

};

