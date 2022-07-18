#include "pch.h"
#include "WaveFrontRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace App;

using namespace DirectX;
using namespace Windows::Foundation;

void WaveFrontRenderer::StartTracking()
{
}
void WaveFrontRenderer::StopTracking()
{
}
// Called once per frame, rotates the cube and calculates the model and view matrices.
void WaveFrontRenderer::Update(DX::StepTimer const& timer)
{
}
void WaveFrontRenderer::TrackingUpdate(float positionX)
{
}

// Renders one frame using the vertex and pixel shaders.
void WaveFrontRenderer::Render()
{
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
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateInputLayout(
					vertexDesc,
					ARRAYSIZE(vertexDesc),
					&fileData[0],
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

// Release memory
void WaveFrontRenderer::ReleaseDeviceDependentResources()
{
	m_waveFrontReader.reset();
}

// Initializes view parameters when the window size changes.
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
	static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

// Loads vertex and pixel shaders from files and instantiates the obj geometry.
WaveFrontRenderer::WaveFrontRenderer(
	const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_indexCount(0),
	m_tracking(false),
	m_loadingComplete(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}