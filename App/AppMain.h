#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\SampleFpsTextRenderer.h"
#include "Content\WaveFrontRenderer.h"

// Renders Direct2D and 3D content on the screen.
namespace App
{
	class AppMain : public DX::IDeviceNotify
	{
	public:
		AppMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~AppMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources>	m_deviceResources;

		// TODO: Replace with your own content renderers.
		std::unique_ptr<SampleFpsTextRenderer>	m_fpsTextRenderer;
		std::unique_ptr<WaveFrontRenderer>		m_waveFrontRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}