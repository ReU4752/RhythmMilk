#ifndef RENDERER_H
#define RENDERER_H

#include <wrl.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <optional>

class Renderer
{
public:
	Renderer();
	~Renderer();

	/* Delete copy constructer */
	Renderer(Renderer const& other) = delete;

	bool InitRenderer(UINT backBufferWidth, UINT backBufferHeight, HWND hOutputWindow);

	void ConfigureBackBuffer(std::optional<std::pair<UINT, UINT>> customBackBufferSize);
	void ReleaseBackBuffer();

	void ToFullScreen();
	void ToWindowed();

	void Clear();

	void Present();

	bool IsInitialized() { return m_isInitilaized; }

private:
	bool m_isInitilaized;

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_dxgiSwapChain;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferRTV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV;

	D3D11_TEXTURE2D_DESC m_backBufferDesc;
	D3D11_VIEWPORT m_viewport;
};

#endif