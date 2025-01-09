#include "DirectXRenderer.h"
#include "structs.h"
#include <stdexcept>

DirectXRenderer::DirectXRenderer(GraphicsDevice* device) {
	this->device = device;
}

DirectXRenderer::~DirectXRenderer() {
	pVS->Release();
	pPS->Release();
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
	device->Device()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &vertexShader);

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
	device->Device()->CreatePixelShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pixelShader);

	if (pVSBlob) {
		pVSBlob->Release();
	}

	return pixelShader;
}

void DirectXRenderer::InitPipeline(void) {
	pVS = LoadVertexShader(L"shaders.shader", "VShader", &vertexBlob);
	pPS = LoadPixelShader(L"shaders.shader", "PShader");

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ID3D11Device* dev = device->Device();
	ID3D11DeviceContext* devcon = device->Context();

	dev->CreateInputLayout(layout, 3, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}

struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

void setMatrixData(ID3D11DeviceContext* devcon, XMMATRIX world, XMMATRIX view, XMMATRIX projection) {
	// Create the matrix buffer.
	MatrixBufferType matrixData;
	matrixData.world = world;       // Assume these matrices are already defined
	matrixData.view = view;
	matrixData.projection = projection;

	// Get a pointer to the buffer in the shader.
	ID3D11Buffer* buffer;
	ID3D11DeviceContext* deviceContext;  // Assume this is already defined
	unsigned int bufferNumber = 0;

	// Lock the buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return;
	}

	// Get a pointer to the data in the buffer.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the buffer.
	*dataPtr = matrixData;

	// Unlock the buffer.
	devcon->Unmap(buffer, 0);

	// Set the buffer in the shader.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &buffer);
}

void DirectXRenderer::Render(void) {
	// do 3D rendering on the back buffer here
	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	ID3D11DeviceContext* devcon = device->Context();

	// set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	// Assume camPosition, camTarget, and camUp are your camera position, target, and up vectors.
	XMVECTOR camPosition = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR camTarget = XMVectorSet(tx, ty, tz, 1.0f);
	XMVECTOR camUp = XMVectorSet(ux, uy, uz, 0.0f);  // Up direction is usually (0, 1, 0)
	XMMATRIX viewMatrix = XMMatrixLookAtLH(camPosition, camTarget, camUp);

	float fovAngleY = XMConvertToRadians(45.0f);  // Field of view angle in the y direction
	float aspectRatio = static_cast<float>(800) / 480;  // Screen width divided by height
	float nearZ = 0.1f;  // Near clipping plane distance
	float farZ = 100.0f;  // Far clipping plane distance
	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

	for (MeshComponent* mesh : meshesToDraw) {
		AssetMesh* asset = mesh->mesh;
		GameObject* obj = mesh->parent;

		float3* pos = &obj->position;
		float3* sca = &obj->scale;
		float4* ori = &obj->orientation;

		// calculate world
		XMVECTOR quaternion = XMVectorSet(ori->x, ori->y, ori->z, ori->w); // Assume x, y, z, w are your quaternion components
		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);
		XMVECTOR position = XMVectorSet(pos->x, pos->y, pos->z, 1.0f); // Assume px, py, pz are your position coordinates
		XMMATRIX translationMatrix = XMMatrixTranslationFromVector(position);
		XMVECTOR scale = XMVectorSet(sca->x, sca->y, sca->z, 1.0f); // Assume sx, sy, sz are your scale factors
		XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);
		XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

		setMatrixData(devcon, worldMatrix, viewMatrix, projectionMatrix);

		// 3 buffers (position, normal, uv)
		devcon->IASetVertexBuffers(0, 3, &asset->pVBuffer, &stride, &offset);

		// select which primtive type we are using
		devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// draw the vertex buffer to the back buffer
		devcon->Draw(asset->vertexCount, 0);
	}
}

AssetMesh* DirectXRenderer::LoadMesh(int count, float4* vertices, float4* normals, float4* uvs) {
	AssetMesh* mesh = new AssetMesh();
	mesh->vertexCount = count;

	VERTEX* dxVertex = new VERTEX[count];
	for (int i = 0; i < count; i++) {
		dxVertex[i] = { vertices[i], uvs[i], normals[i] };
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	ID3D11Device* dev = device->Device();
	ID3D11DeviceContext* devcon = device->Context();

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * count;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	ID3D11Buffer* pVBuffer;
	dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer
	mesh->pVBuffer = pVBuffer;

	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, dxVertex, sizeof(VERTEX) * count);                // copy the data
	devcon->Unmap(pVBuffer, NULL);

	return mesh;
}