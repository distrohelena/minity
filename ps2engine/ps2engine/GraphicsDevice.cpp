#ifdef DIRECTX
#include "GraphicsDevice.h"


GraphicsDevice::GraphicsDevice() {

}

GraphicsDevice::~GraphicsDevice() {
	CleanD3D();
}

// this function initializes and prepares Direct3D for use
void GraphicsDevice::InitD3D(HWND hWnd) {
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	ID3D11Device* baseDevice;
	ID3D11DeviceContext* baseDeviceContext;

	D3D11CreateDevice(nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		D3D11_CREATE_DEVICE_BGRA_SUPPORT, 
		featureLevels, 
		ARRAYSIZE(featureLevels), 
		D3D11_SDK_VERSION, 
		&baseDevice,
		nullptr, 
		&baseDeviceContext);

	baseDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&dev));

	baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&devcon));

	///////////////////////////////////////////////////////////////////////////////////////////////

	IDXGIDevice1* dxgiDevice;
	dev->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&dxgiDevice));

	IDXGIAdapter* dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);
	IDXGIFactory2* dxgiFactory;

	dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory));

	///////////////////////////////////////////////////////////////////////////////////////////////
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	swapChainDesc.Width = 800; // use window width
	swapChainDesc.Height = 600; // use window height
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // prefer DXGI_SWAP_EFFECT_FLIP_DISCARD, see Minimal D3D11 pt2 
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = 0;

	dxgiFactory->CreateSwapChainForHwnd(dev, hWnd, &swapChainDesc, nullptr, nullptr, &swapChain);

	///////////////////////////////////////////////////////////////////////////////////////////////
	ID3D11Texture2D* frameBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&frameBuffer));

	dev->CreateRenderTargetView(frameBuffer, nullptr, &frameBufferView);

	///////////////////////////////////////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	frameBuffer->GetDesc(&depthBufferDesc); // copy from framebuffer properties

	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* depthBuffer;
	dev->CreateTexture2D(&depthBufferDesc, nullptr, &depthBuffer);

	dev->CreateDepthStencilView(depthBuffer, nullptr, &depthBufferView);

	D3D11_RASTERIZER_DESC1 rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	dev->CreateRasterizerState1(&rasterizerDesc, &rasterizerState);

	///////////////////////////////////////////////////////////////////////////////////////////////
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	dev->CreateSamplerState(&samplerDesc, &samplerState);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;


	dev->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	viewport = { 0.0f, 0.0f, static_cast<float>(depthBufferDesc.Width), static_cast<float>(depthBufferDesc.Height), 0.0f, 1.0f };
}

// this is the function that cleans up Direct3D and COM
void GraphicsDevice::CleanD3D()
{
	// close and release all existing COM objects
	swapChain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}

// this is the function used to render a single frame
void GraphicsDevice::BeginRenderFrame(void)
{
	// clear the back buffer to a deep blue
	float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devcon->ClearRenderTargetView(frameBufferView, color);
	devcon->ClearDepthStencilView(depthBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// fill both sides
	devcon->RSSetViewports(1, &viewport);
	devcon->RSSetState(rasterizerState);

	devcon->OMSetRenderTargets(1, &frameBufferView, depthBufferView);
	devcon->OMSetDepthStencilState(depthStencilState, 0);
	devcon->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)
}

void GraphicsDevice::EndRenderFrame(void)
{
	// switch the back buffer and the front buffer
	swapChain->Present(0, 0);
}

#endif