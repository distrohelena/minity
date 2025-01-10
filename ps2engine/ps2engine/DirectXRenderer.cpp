#ifdef DIRECTX

#include "DirectXRenderer.h"
#include "DX_VERTEX.h"
#include <stdexcept>
#include "LightComponent.h"
#include "AssetMaterial.h"
#include "AssetTexture.h"
#include "float3.h"
#include "float4.h"

DirectXRenderer::DirectXRenderer(HWND hWND) {
	this->hwnd = hWND;
}

DirectXRenderer::~DirectXRenderer() {
	pVS->Release();
	pPS->Release();

	CleanD3D();
}

ID3D11VertexShader* DirectXRenderer::LoadVertexShader(LPCWSTR fileName, LPCSTR entryPoint, ID3DBlob** blob) {
	ID3DBlob* pVSBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
		fileName,    // File name
		nullptr,             // Optional macros
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // Optional include handler
		entryPoint,                // Entry point function name
		"vs_4_0",            // Shader model
		0,                   // Shader compile options
		0,                   // Effect compile options
		&pVSBlob,            // Blob for compiled shader
		&pErrorBlob          // Blob for error messages
	);

	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		// Handle the error (e.g., release resources, exit the application, etc.)
		throw std::invalid_argument("Not found");
	}

	// ... Create the shader using the blob, etc.
	ID3D11VertexShader* vertexShader;
	dev->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &vertexShader);

	*blob = pVSBlob;

	return vertexShader;
}

ID3D11PixelShader* DirectXRenderer::LoadPixelShader(LPCWSTR fileName, LPCSTR entryPoint) {
	ID3DBlob* pVSBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
		fileName,    // File name
		nullptr,             // Optional macros
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // Optional include handler
		entryPoint,                // Entry point function name
		"ps_4_0",            // Shader model
		0,                   // Shader compile options
		0,                   // Effect compile options
		&pVSBlob,            // Blob for compiled shader
		&pErrorBlob          // Blob for error messages
	);

	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		// Handle the error (e.g., release resources, exit the application, etc.)
	}

	// ... Create the shader using the blob, etc.
	ID3D11PixelShader* pixelShader;
	dev->CreatePixelShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pixelShader);

	if (pVSBlob) {
		pVSBlob->Release();
	}

	return pixelShader;
}

void DirectXRenderer::InitD3D() {
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

	dxgiFactory->CreateSwapChainForHwnd(dev, hwnd, &swapChainDesc, nullptr, nullptr, &swapChain);

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
	//rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
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

void DirectXRenderer::InitPipeline(void) {
	InitD3D();

	pVS = LoadVertexShader(L"shaders.shader", "VShader", &vertexBlob);
	pPS = LoadPixelShader(L"shaders.shader", "PShader");

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,							  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	dev->CreateInputLayout(layout, numElements, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}

#pragma pack(push, 1)
struct ShaderData
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;

	float3 CameraPos;
	int ActiveLights;

	ShaderLight Lights[10];

	// Define material properties
	float4 materialAmbientColor;
	float4 materialDiffuseColor;
	float3 materialSpecularColor;
	float materialShininess;

	float hasTexture;
};

ID3D11Buffer* DirectXRenderer::setShaderData(ID3D11Device* dev, ID3D11DeviceContext* devcon, XMMATRIX world, XMMATRIX view, XMMATRIX projection, float3 camPos, int activeLights, ShaderLight* lights, AssetMaterial* mat) {
	ID3D11Buffer* constantBuffer = nullptr;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(ShaderData);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT result = dev->CreateBuffer(&bufferDesc, nullptr, &constantBuffer);
	if (FAILED(result))
	{
		// Handle error
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = devcon->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		// Handle error
	}

	ShaderData* data = (ShaderData*)mappedResource.pData;
	data->world = world;
	data->view = view;
	data->projection = projection;
	data->CameraPos = camPos;
	data->ActiveLights = activeLights;

	for (int i = 0; i < 10; i++) {
		data->Lights[i] = lights[i];
	}

	data->materialAmbientColor = { 0.1f, 0.1f, 0.1f, 0.0f };
	data->materialDiffuseColor = mat->color;
	data->materialSpecularColor = { 1.0, 1.0f, 1.0f };
	data->materialShininess = 32.0f;

	data->hasTexture = 0;
	if (mat->texture != nullptr) {
		data->hasTexture = 1;
		// Bind the shader resource view to the pixel shader
		devcon->PSSetShaderResources(0, 1, &mat->texture->srv);
	}

	devcon->Unmap(constantBuffer, 0);

	devcon->VSSetConstantBuffers(0, 1, &constantBuffer);
	devcon->PSSetConstantBuffers(0, 1, &constantBuffer);

	return constantBuffer;
}

void DirectXRenderer::renderCamera(CameraComponent* cam) {
	// do 3D rendering on the back buffer here
	// select which vertex buffer to display
	float3 camPos = cam->parent->GetPosition();
	float3 camRot = cam->parent->GetRotation();

	// Define your quaternion as (x, y, z, w).
	float4 camOri = float3::EulerToQuaternion(camRot);
	XMVECTOR quaternion = XMVectorSet(camOri.x, camOri.y, camOri.z, camOri.w);
	quaternion = XMQuaternionNormalize(quaternion);

	XMVECTOR forward = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), quaternion); // Rotate the forward vector using the quaternion.
	XMVECTOR up = DirectX::XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), quaternion); // Rotate the up vector using the quaternion.

	// Assume camPosition, camTarget, and camUp are your camera position, target, and up vectors.
	XMVECTOR camPosition = XMVectorSet(camPos.x, camPos.y, camPos.z, 1.0f);
	XMVECTOR camTarget = DirectX::XMVectorAdd(camPosition, forward);
	XMMATRIX viewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(camPosition, camTarget, up));

	float fovAngleY = XMConvertToRadians(60);  // Field of view angle in the y direction
	float aspectRatio = 800.0f / 600.0f;  // Screen width divided by height
	XMMATRIX projectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.1f, 1000.0f));
	//XMMATRIX projectionMatrix = XMMatrixOrthographicLH(4, 4, -1000, 1000.0f);

	int ActiveLights = 0;
	ShaderLight Lights[10];
	ZeroMemory(Lights, sizeof(ShaderLight) * 10);

	for (LightComponent* light : lights) {
		Lights[ActiveLights].type = light->type;

		switch (light->type) {
		case 1: {
			// directional
			float4 lightOri = float3::EulerToQuaternion(light->parent->GetRotation());

			XMVECTOR lightQuaternion = XMVectorSet(lightOri.x, lightOri.y, lightOri.z, lightOri.w);
			XMVECTOR lightDir = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), lightQuaternion); // Rotate the forward vector using the quaternion.
			Lights[ActiveLights].direction = { XMVectorGetX(lightDir), XMVectorGetY(lightDir), XMVectorGetZ(lightDir) };
			break;
		}
		default: {
			float3 lightPos = light->parent->GetPosition();
			Lights[ActiveLights].position = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
			break;
		}
		}

		Lights[ActiveLights].color = { light->color.x, light->color.y, light->color.z };
		Lights[ActiveLights].intensity = light->intensity;
		ActiveLights++;
	}

	for (MeshComponent* mesh : meshesToDraw) {
		AssetMesh* asset = mesh->mesh;
		GameObject* obj = mesh->parent;

		float3 pos = obj->GetPosition();
		float3 sca = obj->GetScale();
		float3 rot = obj->GetRotation();
		float4 ori = float3::EulerToQuaternion(rot);

		// calculate world
		XMVECTOR quaternion = XMVectorSet(ori.x, ori.y, ori.z, ori.w); // Assume x, y, z, w are your quaternion components
		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);
		XMVECTOR position = XMVectorSet(pos.x, pos.y, pos.z, 1.0f); // Assume px, py, pz are your position coordinates
		XMMATRIX translationMatrix = XMMatrixTranslationFromVector(position);
		XMVECTOR scale = XMVectorSet(sca.x, sca.y, sca.z, 1.0f); // Assume sx, sy, sz are your scale factors
		XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);
		XMMATRIX worldMatrix = XMMatrixTranspose(scaleMatrix * rotationMatrix * translationMatrix);

		// check if need to update uv
		if (mesh->mat->uvs != nullptr) {
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = devcon->Map(mesh->mesh->vertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
			if (FAILED(hr)) {
				throw;
			}

			VERTEX* dxVertex = reinterpret_cast<VERTEX*>(mappedResource.pData);
			for (int i = 0; i < mesh->mesh->vertexCount; i++) {
				float4 uv = mesh->mat->uvs[i];
				dxVertex[i].uv = { uv.x, uv.y, uv.z, uv.w };
			}

			devcon->Unmap(mesh->mesh->vertexBuffer, 0);
		}

		// set the shader objects
		devcon->VSSetShader(pVS, 0, 0);
		devcon->PSSetShader(pPS, 0, 0);

		ID3D11Buffer* buffer = setShaderData(dev, devcon, worldMatrix, viewMatrix, projectionMatrix, camPos, ActiveLights, Lights, mesh->mat);

		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		devcon->IASetVertexBuffers(0, 1, &asset->vertexBuffer, &stride, &offset);

		// select which primtive type we are using
		devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// draw the vertex buffer to the back buffer
		devcon->Draw(asset->vertexCount, 0);

		buffer->Release();
	}
}

void DirectXRenderer::Render(void) {
	for (CameraComponent* cam : cameras) {
		renderCamera(cam);
	}
}

void DirectXRenderer::LoadTexture(AssetTexture* tex, unsigned int width, unsigned int height, uint8_t pixelWidth) {
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // Assuming 4 bytes per pixel
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = tex->data;  // Your unsigned char* array
	initData.SysMemPitch = width * pixelWidth;  // Assuming pixelWidth is the number of bytes per pixel.

	HRESULT hr = dev->CreateTexture2D(&textureDesc, &initData, &tex->texture);
	if (FAILED(hr)) {
		// Handle the error
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = dev->CreateShaderResourceView(tex->texture, &srvDesc, &tex->srv);
	if (FAILED(hr)) {
		// Handle the error
	}
}

void DirectXRenderer::LoadMesh(AssetMesh* mesh, int totalVerts, float4* vertices, float4* normals, float4* uvs) {
	HRESULT hr;
	if (mesh->vertexBuffer == nullptr) {
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(VERTEX) * totalVerts;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = dev->CreateBuffer(&bufferDesc, nullptr, &mesh->vertexBuffer);
		if (FAILED(hr)) {
			delete mesh;
			throw;
		}
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = devcon->Map(mesh->vertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	if (FAILED(hr)) {
		mesh->vertexBuffer->Release();
		delete mesh;
		throw;
	}

	VERTEX* dxVertex = reinterpret_cast<VERTEX*>(mappedResource.pData);
	if (uvs == nullptr) {
		for (int i = 0; i < totalVerts; i++) {
			dxVertex[i].pos = { vertices[i].x, vertices[i].y, vertices[i].z, vertices[i].w };
			dxVertex[i].normal = { normals[i].x, normals[i].y, normals[i].z, normals[i].w };
		}
	}
	else {
		for (int i = 0; i < totalVerts; i++) {
			dxVertex[i].pos = { vertices[i].x, vertices[i].y, vertices[i].z, vertices[i].w };
			dxVertex[i].uv = { uvs[i].x, uvs[i].y, uvs[i].z, uvs[i].w };
			dxVertex[i].normal = { normals[i].x, normals[i].y, normals[i].z, normals[i].w };
		}
	}


	devcon->Unmap(mesh->vertexBuffer, 0);
}

void DirectXRenderer::BeginRenderFrame(void)
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

void DirectXRenderer::EndRenderFrame(void)
{
	// switch the back buffer and the front buffer
	swapChain->Present(0, 0);
}

void DirectXRenderer::CleanD3D()
{
	// close and release all existing COM objects
	swapChain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}

#endif