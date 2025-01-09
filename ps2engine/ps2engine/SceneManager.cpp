#include "SceneManager.h"
#include "CameraKeyboard.h"
#include "LightComponent.h"
#include "AssetMaterial.h"
#include "AssetTexture.h"

#ifdef PS2
#include <tyra>
using namespace Tyra;
#endif

SceneManager::SceneManager(RenderManager* renderer, ContentManager* content) {
  this->renderer = renderer;
  this->content = content;
}

void SceneManager::ParseAssets(const char* assetsName) {
  // parse assets
  ifstream assetsFile;
  assetsFile.open(assetsName, ios::binary);

  assetsFile.seekg(0, std::ios::end);
  int size = assetsFile.tellg();
  assetsFile.seekg(0);

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

    int current = assetsFile.tellg();
    if (size - current <= 16) {
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

        AssetMesh* mesh = new AssetMesh();
        mesh->vertexCount = vertexCount;

        float4* vertices = new float4[vertexCount];
        float4* normals = new float4[vertexCount];
        unsigned int size = sizeof(float4) * vertexCount;

        assetsFile.read(reinterpret_cast<char*>(vertices), size);
        assetsFile.read(reinterpret_cast<char*>(normals), size);

        bool hasUV;
        assetsFile.read(reinterpret_cast<char*>(&hasUV), 1);

        float4* texcoords = nullptr;
        if (hasUV) {
          texcoords = new float4[vertexCount];
          assetsFile.read(reinterpret_cast<char*>(texcoords), size);
        }

        renderer->LoadMesh(mesh, vertexCount, vertices, normals, texcoords);

        content->RegisterObject(string(guid), mesh);

        break;
      }

      case 2: {
        // material
        AssetMaterial* mat = new AssetMaterial();
        assetsFile.read(reinterpret_cast<char*>(&mat->color), sizeof(float4));

        unsigned char hasTexture;
        assetsFile.read(reinterpret_cast<char*>(&hasTexture), 1);

        if (hasTexture == 1) {
          char texGuid[33];
          assetsFile.read(reinterpret_cast<char*>(&texGuid), 32);
          texGuid[32] = '\0';

          mat->texture = (AssetTexture*)content->FindObject(string(texGuid));
        }

        content->RegisterObject(string(guid), mat);

        break;
      }

      case 3: {
        // texture
        parseTexture(assetsFile, guid, false);

        break;
      }
      case 4: {
        // texture
        parseTexture(assetsFile, guid, true);

        break;
      }
    }
  }

  assetsFile.close();

#ifdef PS2
  TYRA_LOG("SCENEMANAGER: Parsed all assets!");
#endif
}

void SceneManager::parseTexture(ifstream& stream, char guid[33],
                                bool isLightmap) {
  AssetTexture* tex = new AssetTexture();

  if (isLightmap) {
    lightmaps.push_back(tex);
  }

  unsigned short width;
  unsigned short height;
  uint8_t pixelWidth;

  stream.read(reinterpret_cast<char*>(&width), 2);
  stream.read(reinterpret_cast<char*>(&height), 2);
  stream.read(reinterpret_cast<char*>(&pixelWidth), 1);

  unsigned int totalPixelData = width * height * pixelWidth;
  tex->data = new unsigned char[totalPixelData];

  stream.read(reinterpret_cast<char*>(tex->data), totalPixelData);
  renderer->LoadTexture(tex, width, height, pixelWidth);

  content->RegisterObject(string(guid), tex);
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

  unsigned short platform;
  sceneFile.read(reinterpret_cast<char*>(&platform), 2);

  // read lightmap mode
  sceneFile.read(reinterpret_cast<char*>(&lightmap), 2);

  if (lightmap == LightmapMode::BruteForce) {
    uint8_t lightmaps;
    sceneFile.read(reinterpret_cast<char*>(&lightmaps), 1);

    for (int i = 0; i < lightmaps; i++) {
      char guid[33];
      sceneFile.read(reinterpret_cast<char*>(&guid), 32);
      guid[32] = '\0';

      unsigned short assetType;  // lightmap
      sceneFile.read(reinterpret_cast<char*>(&assetType), 2);

      parseTexture(sceneFile, guid, true);
    }
  }

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

#ifdef PS2
  TYRA_LOG("SCENEMANAGER: Parsed all scene!");
#endif
}

GameObject* SceneManager::ReadGameObject(ifstream& stream) {
  GameObject* obj = new GameObject();
  obj->index = counter++;

  stream.read(reinterpret_cast<char*>(&obj->position), sizeof(float3));
  stream.read(reinterpret_cast<char*>(&obj->scale), sizeof(float3));
  stream.read(reinterpret_cast<char*>(&obj->rotation), sizeof(float3));

  uint8_t comps;
  stream.read(reinterpret_cast<char*>(&comps), 1);

  // parse components
  for (int i = 0; i < comps; i++) {
    ComponentType compType;
    stream.read(reinterpret_cast<char*>(&compType), 2);

    switch (compType) {
      case ComponentType::Mesh: {
        MeshComponent* mesh = new MeshComponent();
        stream.read(reinterpret_cast<char*>(&mesh->enabled), 1);

        // mesh
        char guid[33];
        stream.read(reinterpret_cast<char*>(&guid), 32);
        guid[32] = '\0';

        mesh->mesh = (AssetMesh*)content->FindObject(string(guid));
        obj->AddComponent(mesh);

        if (lightmap == LightmapMode::BruteForce) {
          float4* uvs = new float4[mesh->mesh->vertexCount];
          unsigned int size = sizeof(float4) * mesh->mesh->vertexCount;
          stream.read(reinterpret_cast<char*>(uvs), size);

          int lightmapIndex;
          stream.read(reinterpret_cast<char*>(&lightmapIndex), 4);

          AssetMaterial* mat = new AssetMaterial();
          mesh->mat = mat;

          mat->color = {1.0f, 1.0f, 1.0f, 1.0f};
          mat->texture = this->lightmaps.data()[lightmapIndex];
          mat->uvs = uvs;
        } else {
          // material
          char guidMat[33];
          stream.read(reinterpret_cast<char*>(&guidMat), 32);
          guidMat[32] = '\0';
          mesh->mat = (AssetMaterial*)content->FindObject(string(guidMat));
        }

        break;
      }
      case ComponentType::Camera: {
        CameraComponent* cam = new CameraComponent();
        stream.read(reinterpret_cast<char*>(&cam->enabled), 1);
        obj->AddComponent(cam);

        // add camera keyboard
        CameraKeyboard* key = new CameraKeyboard();
        obj->AddComponent(key);

        break;
      }
      case ComponentType::Light: {
        LightComponent* light = new LightComponent();
        stream.read(reinterpret_cast<char*>(&light->enabled), 1);
        obj->AddComponent(light);

        stream.read(reinterpret_cast<char*>(&light->intensity), 4);
        stream.read(reinterpret_cast<char*>(&light->color), sizeof(float4));
        stream.read(reinterpret_cast<char*>(&light->type), 1);

        break;
      }
      case ComponentType::Custom: {
        unsigned short compId;
        stream.read(reinterpret_cast<char*>(&compId), 2);

        

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