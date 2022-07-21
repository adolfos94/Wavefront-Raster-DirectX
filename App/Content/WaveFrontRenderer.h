#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderStructures.h"
#include "WaveFrontReader.h"

namespace App
{
	// This sample renderer instantiates a basic rendering pipeline.
	class WaveFrontRenderer
	{
	public:
		WaveFrontRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

	private:

	private:

		Concurrency::task<void> CreateVertexShaderLayout();
		Concurrency::task<void> CreatePixelShaderLayout();
		Concurrency::task<void> CreateOBJFile();
		Concurrency::task<void> CreateMesh(
			Concurrency::task<void>& vertexShaderTask,
			Concurrency::task<void>& pixelShaderTask);

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources>		m_deviceResources;

		// Direct3D resources for geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for geometry.
		std::unique_ptr<WaveFrontReader<uint32_t>>	m_waveFrontReader;
		ModelViewProjectionConstantBuffer			m_constantBufferData;
		size_t m_vertexCount;
		size_t m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
	};
}
