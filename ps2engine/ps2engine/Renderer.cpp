#include "Renderer.h"
#include "structs.h"
#include <stdexcept>

Renderer::Renderer(GraphicsDevice* device) {
	this->device = device;
}

Renderer::~Renderer() {
	pVS->Release();
	pPS->Release();
}

ID3D11VertexShader* Renderer::LoadVertexShader(LPCWSTR fileName, LPCSTR entryPoint, ID3DBlob** blob) {
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
	device->Device()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &vertexShader);

	*blob = pVSBlob;

	return vertexShader;
}

ID3D11PixelShader* Renderer::LoadPixelShader(LPCWSTR fileName, LPCSTR entryPoint) {
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
	device->Device()->CreatePixelShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pixelShader);

	if (pVSBlob) {
		pVSBlob->Release();
	}

	return pixelShader;
}

void Renderer::InitPipeline(void) {
	ID3DBlob* vertexBlob = nullptr;
	pVS = LoadVertexShader(L"shaders.shader", "VShader", &vertexBlob);
	pPS = LoadPixelShader(L"shaders.shader", "PShader");

	VERTEX OurVertices[] =
	{
		{0.0f, 0.5f, 0.0f,  DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
		{0.45f, -0.5, 0.0f,  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
		{-0.45f, -0.5f, 0.0f,  DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	ID3D11Device* dev = device->Device();
	ID3D11DeviceContext* devcon = device->Context();
	dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer

	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));                // copy the data
	devcon->Unmap(pVBuffer, NULL);

	// set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	dev->CreateInputLayout(ied, 2, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);

	if (vertexBlob) {
		vertexBlob->Release();
	}
}

void Renderer::Render(void) {
	// do 3D rendering on the back buffer here
	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	ID3D11DeviceContext* devcon = device->Context();
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

	// select which primtive type we are using
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	devcon->Draw(3, 0);
}