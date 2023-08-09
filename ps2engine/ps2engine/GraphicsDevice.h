#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3d10.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

class GraphicsDevice
{
public: 
	GraphicsDevice();
	~GraphicsDevice();

	// function prototypes
	void InitD3D(HWND hWnd);     // sets up and initializes Direct3D
	void BeginRenderFrame(void);
	void EndRenderFrame(void);

	ID3D11Device* Device() {
		return dev;
	}
	ID3D11DeviceContext* Context() {
		return devcon;
	}

private:
	// global declarations
	IDXGISwapChain* swapchain;             // the pointer to the swap chain interface
	ID3D11Device* dev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext* devcon;           // the pointer to our Direct3D device context
	ID3D11RenderTargetView* backbuffer;    // global declaration

	void CleanD3D(void);         // closes Direct3D and releases memory
};
