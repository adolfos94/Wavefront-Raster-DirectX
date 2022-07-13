#include "pch.h"
#include "ModelClass.h"

using namespace DirectX;

HRESULT ModelClass::InitializeBuffers(ID3D11Device* device)
{
	HRESULT result;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	m_vertexCount = m_WaveFrontReader.vertices.size();
	m_indexCount = m_WaveFrontReader.indices.size();

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(WaveFrontReader<uint32_t>::Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_WaveFrontReader.vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the Vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return E_FAIL;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = m_WaveFrontReader.indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the Index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return E_FAIL;

	m_WaveFrontReader;

	return S_OK;
}

HRESULT ModelClass::LoadModel(const wchar_t* szFileName)
{
	m_WaveFrontReader = WaveFrontReader<uint32_t>();

	return m_WaveFrontReader.Load(szFileName);
}

bool ModelClass::Initialize(ID3D11Device* device, const wchar_t* szFileName)
{
	// Load WaveFront Model
	LoadModel(szFileName);

	InitializeBuffers(device);

	return false;
}

ModelClass::ModelClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}