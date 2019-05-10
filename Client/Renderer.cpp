#include "Renderer.hpp"

Renderer::Renderer()
	: m_isInitilaized(), m_device(), m_context(), m_dxgiSwapChain(), m_backBuffer(), m_backBufferRTV()
{
}

Renderer::~Renderer()
{
}

bool Renderer::InitRenderer(UINT backBufferWidth, UINT backBufferHeight, HWND hOutputWindow)
{
	HRESULT hr;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL  featureLevelsSupported;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = backBufferWidth;
	sd.BufferDesc.Height = backBufferHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hOutputWindow;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> dxgiSwapChain;
	
	hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			featureLevels,
			_countof(featureLevels),
			D3D11_SDK_VERSION,
			&sd,
			dxgiSwapChain.GetAddressOf(),
			device.GetAddressOf(),
			&featureLevelsSupported,
			context.GetAddressOf());

	// If computer only support Direct3D 11.0 not Direct3D 11.1, Recreate device without 'D3D_FEATURE_LEVEL_11_1'
	// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-devices-initialize
	if (hr == E_INVALIDARG)
	{
		hr = D3D11CreateDeviceAndSwapChain(
				nullptr, 
				D3D_DRIVER_TYPE_HARDWARE, 
				nullptr, 
				createDeviceFlags, 
				&featureLevels[1],
				_countof(featureLevels) - 1,
				D3D11_SDK_VERSION, 
				&sd, 
				dxgiSwapChain.GetAddressOf(),
				device.GetAddressOf(),
				&featureLevelsSupported,
				context.GetAddressOf());
	}

	if FAILED(hr)
		return false;

	device.As(&m_device);
	context.As(&m_context);
	dxgiSwapChain.As(&m_dxgiSwapChain);

	// Get a pointer to the back buffer
	hr = dxgiSwapChain->GetBuffer(
			0, 
			__uuidof(ID3D11Texture2D),
			(void**)&m_backBuffer);

	if SUCCEEDED(hr)
	{
		m_backBuffer->GetDesc(&m_backBufferDesc);

		hr = m_device->CreateRenderTargetView(
			m_backBuffer.Get(),
			nullptr,
			m_backBufferRTV.GetAddressOf()
			);

		m_context->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), NULL);
	}

	// Setup the viewport
	m_viewport.Width = 640;
	m_viewport.Height = 480;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;

	m_context->RSSetViewports(1, &m_viewport);

	m_isInitilaized = true;

	return true;
}

void Renderer::ConfigureBackBuffer(std::optional<std::pair<UINT, UINT>> customBackBufferSize)
{
	HRESULT hr;

	// Create BackBuffer
	hr = m_dxgiSwapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			(void**)&m_backBuffer);

	hr = m_device->CreateRenderTargetView(
		m_backBuffer.Get(),
		nullptr,
		m_backBufferRTV.GetAddressOf()
	);

	// Create depth stencil buffer
	m_backBuffer->GetDesc(&m_backBufferDesc);

	if (customBackBufferSize.has_value())
	{
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			customBackBufferSize->first,
			customBackBufferSize->second,
			1, // This depth stencil view has only one texture.
			1, // Use a single mipmap level.
			D3D11_BIND_DEPTH_STENCIL
		);
	}

	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT>(m_backBufferDesc.Width),
		static_cast<UINT>(m_backBufferDesc.Height),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
		);

	m_device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&m_depthStencil
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	m_device->CreateDepthStencilView(
		m_depthStencil.Get(),
		&depthStencilViewDesc,
		&m_DSV
	);

	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
	m_viewport.Width = m_backBufferDesc.Height;
	m_viewport.Height = m_backBufferDesc.Width;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;

	m_context->RSSetViewports(1, &m_viewport);
}

void Renderer::ReleaseBackBuffer()
{
	// Release the render target view based on the back buffer.
	m_backBufferRTV.Reset();

	// Release the back buffer itself
	m_backBuffer.Reset();

	// The depth stencil will need to be resized, so release it (and view)
	m_DSV.Reset();
	m_depthStencil.Reset();

	// After releasing references to these resources, we need to call Flush() to 
	// ensure that Direct3D also releases any references it might still have to
	// the same resources - such as pipeline bindings.
	m_context->Flush();
}

void Renderer::Clear()
{
	FLOAT clearColor[4] = { 0.0f, 1.0f , 1.0f , 1.0f };
	m_context->ClearRenderTargetView(m_backBufferRTV.Get(), clearColor);
}

void Renderer::Present()
{
	m_dxgiSwapChain->Present(0, 0);
}