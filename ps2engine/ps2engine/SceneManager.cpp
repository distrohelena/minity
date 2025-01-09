#include "SceneManager.h"
#include <cstdint>
#include "MeshComponent.h"

SceneManager::SceneManager(RenderManager* renderer, ContentManager* content) {
	this->renderer = renderer;
	this->content = content;
}

void SceneManager::ParseAssets(const char* assetsName) {
	// parse assets
	ifstream assetsFile;
	assetsFile.open(assetsName, ios::binary);

	// parse
	char nuke[4];
	assetsFile.read(nuke, 4);

	unsigned int type;
	assetsFile.read(reinterpret_cast<char*>(&type), 4);

	unsigned int version;
	assetsFile.read(reinterpret_cast<char*>(&version), 4);

	for (;;) {
		if (assetsFile.eof()) {
			break;
		}

		char guid[33];
		assetsFile.read(reinterpret_cast<char*>(&guid), 32);
		guid[32] = '\0';

		unsigned short assetType;
		assetsFile.read(reinterpret_cast<char*>(&assetType), 2);

		switch (assetType) {
		case 1: {
			// mesh
			char meshType;
			assetsFile.read(&meshType, 1);

			if (meshType != 120) {
				throw;
			}

			unsigned int vertexCount;
			assetsFile.read(reinterpret_cast<char*>(&vertexCount), 4);

			float4* vertices = new float4[vertexCount];
			float4* normals = new float4[vertexCount];
			float4* uvs = new float4[vertexCount];

			unsigned int size = sizeof(float4) * vertexCount;
			assetsFile.read(reinterpret_cast<char*>(vertices), size);
			assetsFile.read(reinterpret_cast<char*>(normals), size);
			assetsFile.read(reinterpret_cast<char*>(uvs), size);

			auto pos = assetsFile.tellg();

			AssetMesh* mesh = renderer->LoadMesh(vertexCount, vertices, normals, uvs);
			content->RegisterObject(string(guid), mesh);

			break;
		}

		}
	}

	assetsFile.close();
}

void SceneManager::ParseScene(const char* sceneName) {
	// parse assets
	ifstream sceneFile;
	sceneFile.open(sceneName, ios::binary);

	// parse
	char nuke[4];
	sceneFile.read(nuke, 4);

	unsigned int type;
	sceneFile.read(reinterpret_cast<char*>(&type), 4);

	unsigned int version;
	sceneFile.read(reinterpret_cast<char*>(&version), 4);

	for (;;) {
		if (sceneFile.eof()) {
			break;
		}

		unsigned int gameObjects;
		if (!sceneFile.read(reinterpret_cast<char*>(&gameObjects), 4)) {
			break;
		}

		for (int i = 0; i < gameObjects; i++) {
			GameObject* object = ReadGameObject(sceneFile);
			rootObjs.push_back(object);
		}
	}

	sceneFile.close();
}

GameObject* SceneManager::ReadGameObject(ifstream& stream)
{
	GameObject* obj = new GameObject();
	stream.read(reinterpret_cast<char*>(&obj->position), sizeof(float3));
	stream.read(reinterpret_cast<char*>(&obj->scale), sizeof(float3));
	stream.read(reinterpret_cast<char*>(&obj->orientation), sizeof(float4));

	uint8_t comps;
	stream.read(reinterpret_cast<char*>(&comps), 1);

	// parse components
	for (int i = 0; i < comps; i++) {
		unsigned short compType;
		stream.read(reinterpret_cast<char*>(&compType), 2);

		switch (compType) {
		case 1: {
			// mesh
			char guid[33];
			stream.read(reinterpret_cast<char*>(&guid), 32);
			guid[32] = '\0';

			MeshComponent* mesh = new MeshComponent();
			mesh->mesh = (AssetMesh*)content->FindObject(string(guid));
			obj->AddComponent(mesh);

			break;
		}
		}
	}

	int children;
	stream.read(reinterpret_cast<char*>(&children), 4);
	for (int i = 0; i < children; i++) {
		GameObject* child = ReadGameObject(stream);
		obj->AddChild(child);
	}

	return obj;
}