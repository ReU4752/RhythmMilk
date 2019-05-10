#include "Renderer.hpp"

#include <stdio.h>

#if defined (_WIN32) || defined(_WIN64)
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool Init();
void Render();

HWND g_mainHWnd;
Renderer g_renderer;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Metronome";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	g_mainHWnd = CreateWindow("Metronome", "Metronome", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		500,
		300,
		NULL,
		(HMENU)NULL,
		hInstance,
		NULL);

	if (g_mainHWnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Init
	if (!Init())
	{
		DestroyWindow(g_mainHWnd);
		return 0;
	}

	ShowWindow(g_mainHWnd, nCmdShow);
	UpdateWindow(g_mainHWnd);

	bool quit = false;
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (!quit)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				quit = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		Render();
	}

	DestroyWindow(g_mainHWnd);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

bool Init()
{
	RECT clientRect;
	GetClientRect(g_mainHWnd, &clientRect);

	g_renderer.InitRenderer(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, g_mainHWnd);

	return true;
}

void Render()
{
	g_renderer.Clear();

	g_renderer.Present();
}

#endif