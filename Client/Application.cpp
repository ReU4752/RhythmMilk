#include "Application.hpp"

Application::Application()
{
}

Application::~Application()
{
}

void Application::Init(HWND hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	m_renderer.InitRenderer(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, hWnd);
}

void Application::Update()
{
}

void Application::Render()
{
	m_renderer.Clear();

	m_renderer.Present();
}

void Application::Release()
{
}