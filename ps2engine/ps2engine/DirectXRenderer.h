#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "GraphicsDevice.h"
#include "AssetMesh.h"
#include "structs.h"
#include "GameObject.h"
#include "RenderManager.h"
#include "MeshComponent.h"
#include <DirectXMath.h>
using namespace DirectX;

class RenderMesh {
	AssetMesh* mesh;
	GameObject* object;
};

class DirectXRenderer : public RenderManager
{
public:
	DirectXRenderer(GraphicsDevice* device);
	~DirectXRenderer();

	ID3D11VertexShader* LoadVertexShader(LPCWSTR fileName, LPCSTR entryPoint, ID3DBlob** blob);
	ID3D11PixelShader* LoadPixelShader(LPCWSTR fileName, LPCSTR entryPoint);

	void InitPipeline(void) override;
	AssetMesh* LoadMesh(int count, float4* vertices, float4* normals, float4* uvs) override;
	void Render(void);

private:
	GraphicsDevice* device;

	ID3D11VertexShader* pVS;    // the vertex shader
	ID3D11PixelShader* pPS;     // the pixel shader

	ID3DBlob* vertexBlob;
	ID3D11InputLayout* pLayout;
public:

	DirectXRenderer() = default;
};

