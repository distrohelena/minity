#pragma once

#if DIRECTX

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d10.h>
#include <d3dcompiler.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "user32")

#include "AssetMesh.h"
#include "structs.h"
#include "GameObject.h"
#include "RenderManager.h"
#include "MeshComponent.h"
#include"CameraComponent.h"
#include <DirectXMath.h>
using namespace DirectX;

class RenderMesh {
	AssetMesh* mesh;
	GameObject* object;
};

struct ShaderLight {
	float4 position; // light position, for point and spot lights
	float4 direction; // light direction, for directional and spot lights
	float4 color; // light color
	int type; // light type: directional, point, spot
	float range; // light range, for point and spot lights
	float spotAngle; // spot angle in degrees, for spot lights
	float intensity; // light intensity
};

class DirectXRenderer : public RenderManager
{
public:
	DirectXRenderer(HWND hwnd);
	~DirectXRenderer();

	ID3D11VertexShader* LoadVertexShader(LPCWSTR fileName, LPCSTR entryPoint, ID3DBlob** blob);
	ID3D11PixelShader* LoadPixelShader(LPCWSTR fileName, LPCSTR entryPoint);

	void InitPipeline(void) override;
	void LoadMesh(AssetMesh* mesh, int totalVerts, float4* vertices, float4* normals, float4* uvs) override;
	void LoadTexture(AssetTexture* tex, unsigned int width, unsigned int height, uint8_t pixelWidth) override;
	void Render(void) override;

	void BeginRenderFrame(void) override;
	void EndRenderFrame(void) override;

private:
	HWND hwnd;

	void InitD3D(void);

	// global declarations
	IDXGISwapChain1* swapChain;             // the pointer to the swap chain interface
	ID3D11Device1* dev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext1* devcon;           // the pointer to our Direct3D device context
	ID3D11RenderTargetView* backbuffer;    // global declaration
	ID3D11RasterizerState1* rasterizerState;
	ID3D11SamplerState* samplerState;
	ID3D11DepthStencilView* depthBufferView;
	ID3D11RenderTargetView* frameBufferView;
	ID3D11DepthStencilState* depthStencilState;
	D3D11_VIEWPORT viewport;

	ID3D11VertexShader* pVS;    // the vertex shader
	ID3D11PixelShader* pPS;     // the pixel shader

	ID3DBlob* vertexBlob;
	ID3D11InputLayout* pLayout;

	ID3D11Buffer* setShaderData(ID3D11Device* dev, ID3D11DeviceContext* devcon, XMMATRIX world, XMMATRIX view, XMMATRIX projection, float3 camPos, int activeLights, ShaderLight* lights, AssetMaterial* mat);
	void renderCamera(CameraComponent* cam);
	void CleanD3D(void);         // closes Direct3D and releases memory
};

#endif