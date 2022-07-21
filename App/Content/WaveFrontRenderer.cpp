#include "pch.h"
#include "WaveFrontRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace App;

using namespace DirectX;
using namespace Windows::Foundation;

// Called once per frame, rotates the mesh and calculates the model matrices.
void WaveFrontRenderer::Update(DX::StepTimer const& timer)
{
	XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixIdentity());
}

// Renders one frame using the vertex and pixel shaders.
void WaveFrontRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
		return;

	// Pipeline state and generate rendering commands using the resources owned by a device
	auto context = m_deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset);

	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0);

	// Bind information about the primitive type, and data order that describes input data for the input.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Describe the input - buffer data for the input vertexes.
	context->IASetInputLayout(m_inputLayout.Get());

	// Attach the vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0);

	// Attach the pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr);

	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);
}

// Create the vertex shader and input layout.
Concurrency::task<void> WaveFrontRenderer::CreateVertexShaderLayout()
{
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	return loadVSTask.then([this](const std::vector<byte>& fileData)
		{
			// Vertex Shader
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateVertexShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					&m_vertexShader)
			);

			// Vertex Input Layout
			static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateInputLayout(
					vertexDesc,
					ARRAYSIZE(vertexDesc),
					fileData.data(),
					fileData.size(),
					&m_inputLayout)
			);
		});
}

// Create the pixel shader and constant buffer.
Concurrency::task<void> WaveFrontRenderer::CreatePixelShaderLayout()
{
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");
	return loadPSTask.then([this](const std::vector<byte>& fileData)
		{
			// Pixel Shader
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreatePixelShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					&m_pixelShader)
			);

			// Constant Buffer Model View Projection
			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateBuffer(
					&constantBufferDesc,
					nullptr,
					&m_constantBuffer)
			);
		});
}

// Create the mesh.
Concurrency::task<void> WaveFrontRenderer::CreateMesh(
	Concurrency::task<void>& vertexShaderTask,
	Concurrency::task<void>& pixelShaderTask)
{
	return (vertexShaderTask && pixelShaderTask).then([this]()
		{
			m_waveFrontReader = std::unique_ptr<WaveFrontReader<uint32_t>>(new WaveFrontReader<uint32_t>());
			m_waveFrontReader->Load(L"./Assets/impart.txt");

			// Mesh vertices.
			m_vertexCount = m_waveFrontReader->vertices.size();

			if (!m_vertexCount)
				return;

			D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
			vertexBufferData.pSysMem = m_waveFrontReader->vertices.data();
			vertexBufferData.SysMemPitch = 0;
			vertexBufferData.SysMemSlicePitch = 0;

			CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionNormal) * m_vertexCount, D3D11_BIND_VERTEX_BUFFER);
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateBuffer(
					&vertexBufferDesc,
					&vertexBufferData,
					&m_vertexBuffer)
			);

			// Mesh indices.
			m_indexCount = m_waveFrontReader->indices.size();

			if (!m_indexCount)
				return;

			D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
			indexBufferData.pSysMem = m_waveFrontReader->indices.data();
			indexBufferData.SysMemPitch = 0;
			indexBufferData.SysMemSlicePitch = 0;

			CD3D11_BUFFER_DESC indexBufferDesc(sizeof(uint32_t) * m_indexCount, D3D11_BIND_INDEX_BUFFER);
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateBuffer(
					&indexBufferDesc,
					&indexBufferData,
					&m_indexBuffer)
			);
		});
}

// Initializes Meshes, Shaders and Render buffers
void WaveFrontRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto createVSTask = CreateVertexShaderLayout();
	auto createPSTask = CreatePixelShaderLayout();

	// Create the mesh.
	auto createMeshTask = CreateMesh(createVSTask, createPSTask);

	// Once the mesh is loaded, the object is ready to be rendered.
	createMeshTask.then([this]()
		{
			m_loadingComplete = true;
		});
}

// Initializes view parameters and Constant Buffer Data.
void WaveFrontRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	if (aspectRatio < 1.0f)
		fovAngleY *= 2.0f;

	// Perspective Projection Matrix.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);

	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(perspectiveMatrix));

	// View Matrix
	static const XMVECTORF32 eye = { 1.0f, 0.7f, 0.1, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f,  1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

// Release memory
void WaveFrontRenderer::ReleaseDeviceDependentResources()
{
	// Vertex Shader and Layout
	m_vertexShader.Reset();
	m_inputLayout.Reset();

	// Pixel Shader and ConstantBuffer
	m_pixelShader.Reset();
	m_constantBuffer.Reset();

	// Mesh Info
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
	m_waveFrontReader.reset();

	m_loadingComplete = false;
}

// Loads vertex and pixel shaders from files and instantiates the obj geometry.
WaveFrontRenderer::WaveFrontRenderer(
	const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_indexCount(0),
	m_vertexCount(0),
	m_loadingComplete(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}