#include "TyraRenderManager.h"
#include "LightComponent.h"
#include "AssetMaterial.h"
#include "AssetTexture.h"

#ifdef PS2
TyraRenderManager::TyraRenderManager(Engine* eng) { engine = eng; }

TyraRenderManager::~TyraRenderManager() {}

void TyraRenderManager::InitPipeline() {
  stapip.setRenderer(&engine->renderer.core);
  renderOptions.frustumCulling = Tyra::PipelineFrustumCulling_Precise;
  renderOptions.fullClipChecks = true;
}

void TyraRenderManager::LoadTexture(AssetTexture* tex, unsigned int width,
                                    unsigned int height, uint8_t pixelWidth) {
  TextureBuilderData* data = new TextureBuilderData();
  data->width = width;
  data->height = height;
  data->gsComponents = TEXTURE_COMPONENTS_RGBA;
  data->bpp = bpp32;
  data->data = tex->data;

  Texture* texture = new Texture(data);
  tex->texture = texture;
  delete data;
}

void TyraRenderManager::LoadMesh(AssetMesh* mesh, int totalVerts,
                                 float4* vertices, float4* normals,
                                 float4* uvs) {
  MeshBuilderData* data = new MeshBuilderData();
  data->loadNormals = true;

  MeshBuilderMaterialData* mat = new MeshBuilderMaterialData();
  mat->name = "test";

  MeshBuilderMaterialFrameData* frame = new MeshBuilderMaterialFrameData();
  frame->normals = reinterpret_cast<Vec4*>(normals);
  frame->vertices = reinterpret_cast<Vec4*>(vertices);
  frame->textureCoords = reinterpret_cast<Vec4*>(uvs);
  frame->count = totalVerts;

  mat->frames.push_back(frame);

  data->materials.push_back(mat);

  mesh->mesh = std::make_unique<StaticMesh>(data);
}

void TyraRenderManager::Render(void) {
  for (CameraComponent* cam : cameras) {
    renderCamera(cam);
  }

  if (cameras.size() == 0) {
    engine->renderer.beginFrame();
    engine->renderer.endFrame();
  }
}

void TyraRenderManager::BeginRenderFrame(void) {}

void TyraRenderManager::EndRenderFrame(void) {}

M4x4 TyraRenderManager::createFromQuaternion(float4 quaternion) {
  float num9 = quaternion.x * quaternion.x;
  float num8 = quaternion.y * quaternion.y;
  float num7 = quaternion.z * quaternion.z;
  float num6 = quaternion.x * quaternion.y;
  float num5 = quaternion.z * quaternion.w;
  float num4 = quaternion.z * quaternion.x;
  float num3 = quaternion.y * quaternion.w;
  float num2 = quaternion.y * quaternion.z;
  float num = quaternion.x * quaternion.w;
  M4x4 result(1.0f - (2.0f * (num8 + num7)), 2.0f * (num6 + num5),
              2.0f * (num4 - num3), 0.0f, 2.0f * (num6 - num5),
              1.0f - (2.0f * (num7 + num9)), 2.0f * (num2 + num), 0.0f,
              2.0f * (num4 + num3), 2.0f * (num2 - num),
              1.0f - (2.0f * (num8 + num9)), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  return result;
}

void TyraRenderManager::setLightOptions(float4 color) {
  // directionalAmbientColor.set(100.0F, 100.0F, 100.0F);
  directionalAmbientColor.set(100.0F * color.x, 100.0F * color.y,
                              100.0F * color.z);
  // Reset colors and directions
  for (int i = 0; i < 3; i++) {
    directionalColors[i].set(0.0F, 0.0F, 0.0F, 1.0F);
    directionalDirections[i].set(1.0F, 1.0F, 1.0F, 1.0F);
  }

  int ActiveLights = 0;
  for (LightComponent* light : lights) {
    float3 lightDir = {0, 0, 1};
    lightDir.Transform(float3::EulerToQuaternion(light->parent->GetRotation()));

    float4 lightColor = light->color;

    // directionalColors[ActiveLights].set(
    //     lightColor.x * 255.0f, lightColor.y * 255.0f, lightColor.z * 255.0f,
    //     lightColor.w * 255.0f);
    directionalColors[ActiveLights].set(
        lightColor.x * color.x * 255.0f, lightColor.y * color.y * 255.0f,
        lightColor.z * color.z * 255.0f, lightColor.w * 255.0f);
    // directionalDirections[ActiveLights].set(-0.5f, -1.0f, -0.5f, 1.0F);
    directionalDirections[ActiveLights].set(lightDir.x, lightDir.y, lightDir.z,
                                            1.0f);

    ActiveLights++;

    if (ActiveLights == 3) {
      break;
    }
  }

  // set lights
  renderLightingOptions.ambientColor = &directionalAmbientColor;
  renderLightingOptions.directionalColors = directionalColors.begin();
  renderLightingOptions.directionalDirections = directionalDirections.begin();
}

void TyraRenderManager::renderCamera(CameraComponent* cam) {
  float3 position = cam->parent->GetPosition();
  float3 forward = {0, 0, 1};
  forward.Transform(float3::EulerToQuaternion(cam->parent->GetRotation()));
  forward.Add(position);

  Vec4 pos = {position.x, position.y, position.z, 1.0f};
  Vec4 fwd = {forward.x, forward.y, forward.z, 1.0f};

  CameraInfo3D info(&pos, &fwd);

  engine->renderer.beginFrame(info);
  engine->renderer.renderer3D.usePipeline(stapip);

  renderOptions.shadingType = TyraShadingGouraud;
  renderOptions.blendingEnabled = true;
  renderOptions.lighting = &renderLightingOptions;

  // setLightOptions();

  for (MeshComponent* mesh : meshesToDraw) {
    AssetMesh* asset = mesh->mesh;
    GameObject* obj = mesh->parent;

    float3 pos = obj->GetPosition();
    float3 sca = obj->GetScale();
    float3 rot = obj->GetRotation();
    rot.ToRadians();

    StaticMesh* tyraMesh = asset->mesh.get();
    tyraMesh->setPosition({-pos.x, pos.y, pos.z, 1.0f});

    M4x4 scaMatrix = M4x4::Identity;
    scaMatrix.data[0] = -sca.x;
    scaMatrix.data[5] = sca.y;
    scaMatrix.data[10] = sca.z;
    scaMatrix.data[15] = 1.0F;
    tyraMesh->scale = scaMatrix;

    M4x4 rotMatrix = M4x4::Identity;
    rotMatrix.rotate({rot.x, -rot.y, rot.z});
    tyraMesh->rotation = rotMatrix;

    MeshMaterial* mat = tyraMesh->materials[0];
    Vec4* uvs = mat->frames[0]->textureCoords;
    float4 color = mesh->mat->color;

    if (mesh->mat->uvs == nullptr) {
      if (mesh->mat->texture == nullptr) {
        mat->texture = nullptr;
      } else {
        mat->texture = mesh->mat->texture->texture;
      }
      mat->ambient = {color.x * 128.0f, color.y * 128.0f, color.z * 128.0f,
                      color.w * 128.0f};
    } else {
      // lightmaps
      mat->texture = mesh->mat->texture->texture;
      mat->frames[0]->textureCoords = reinterpret_cast<Vec4*>(mesh->mat->uvs);
      mat->ambient = {color.x * 255.0f, color.y * 255.0f, color.z * 255.0f,
                      255.0F};
    }

    mat->lightmapFlag = false;

    setLightOptions(color);

    renderLightingOptions.ambientColor = &directionalAmbientColor;
    renderLightingOptions.directionalColors = directionalColors.begin();
    renderLightingOptions.directionalDirections = directionalDirections.begin();

    stapip.render(tyraMesh, renderOptions);

    mat->frames[0]->textureCoords = uvs;
  }

  engine->renderer.endFrame();
}

#endif
