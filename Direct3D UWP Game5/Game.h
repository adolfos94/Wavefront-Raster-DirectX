//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "ModelClass.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

	Game() noexcept(false);
	~Game() = default;

	Game(Game&&) = default;
	Game& operator= (Game&&) = default;

	Game(Game const&) = delete;
	Game& operator= (Game const&) = delete;

	// Initialization and management
	void Initialize(IUnknown* window, int width, int height, DXGI_MODE_ROTATION rotation);

	// Basic game loop
	void Tick();

	// IDeviceNotify
	void OnDeviceLost() override;
	void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnDisplayChange();
	void OnWindowSizeChanged(int width, int height, DXGI_MODE_ROTATION rotation);
	void ValidateDevice();

	// Properties
	void GetDefaultSize(int& width, int& height) const noexcept;

private:

	void Update(DX::StepTimer const& timer);
	void Render();
	void RenderModel();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
	std::unique_ptr<DirectX::Model> m_model;

	// Device resources.
	std::unique_ptr<DX::DeviceResources>    m_deviceResources;

	ModelClass model;

	// Rendering loop timer.
	DX::StepTimer                           m_timer;
};
