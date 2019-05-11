#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Renderer.hpp"

class Application
{
public:
	Application();
	~Application();

	void Init(HWND hWnd);

	void Update();

	void Render();

	void Release();

private:
	Renderer m_renderer;
};

#endif