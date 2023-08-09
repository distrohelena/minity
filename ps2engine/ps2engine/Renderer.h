#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "GraphicsDevice.h"

class Renderer
{
public:
	Renderer(GraphicsDevice* device);
	~Renderer();

	void InitPipeline(void);
	ID3D11VertexShader* LoadVertexShader(LPCWSTR fileName, LPCSTR entryPoint, ID3DBlob** blob);
	ID3D11PixelShader* LoadPixelShader(LPCWSTR fileName, LPCSTR entryPoint);
	void Render(void);

private:
	GraphicsDevice* device;
	ID3D11InputLayout* pLayout;

	ID3D11VertexShader* pVS;    // the vertex shader
	ID3D11PixelShader* pPS;     // the pixel shader
	ID3D11Buffer* pVBuffer;    // global

};

