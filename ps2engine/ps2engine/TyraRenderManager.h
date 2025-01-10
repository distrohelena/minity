#pragma once

#ifdef PS2

#include "AssetMesh.h"
#include "float3.h"
#include "float4.h"
#include "GameObject.h"
#include "RenderManager.h"
#include "MeshComponent.h"
#include "CameraComponent.h"

#include <tyra>

using namespace Tyra;

class TyraRenderManager : public RenderManager {
public:
    TyraRenderManager(Engine* eng);
    ~TyraRenderManager();

    void InitPipeline(void) override;
    void LoadMesh(AssetMesh* mesh, int totalVerts, float4* vertices,
        float4* normals, float4* uvs) override;
    void LoadTexture(AssetTexture* tex, unsigned int width,
        unsigned int height,
        uint8_t pixelWidth) override;
    void Render(void) override;

    void BeginRenderFrame(void) override;
    void EndRenderFrame(void) override;

private:
    M4x4 createFromQuaternion(float4 quaternion);
    void renderCamera(CameraComponent* cam);

    void setLightOptions(float4 color);

    Engine* engine;
    /** 3D pipeline used for rendering static meshes */
    StaticPipeline stapip;
    /** Options for static pipeline */
    PipelineLightingOptions renderLightingOptions;
    StaPipOptions renderOptions;

    Color directionalAmbientColor;
    std::array<Color, 3> directionalColors;
    std::array<Vec4, 3> directionalDirections;
};

#endif
