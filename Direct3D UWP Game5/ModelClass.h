#pragma once

class ModelClass
{
public:

	ModelClass();

	bool Initialize(ID3D11Device* device, const wchar_t* szFileName);
	void Render(ID3D11DeviceContext*);
	void Shutdown();

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;
	};

	HRESULT LoadModel(const wchar_t* szFileName);
	HRESULT InitializeBuffers(ID3D11Device* device);
	void RenderBuffers(ID3D11DeviceContext*);

	void ShutdownBuffers();

	void ReleaseModel();

	WaveFrontReader<uint32_t> m_WaveFrontReader;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int	m_indexCount;
};
