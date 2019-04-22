#include <cstdlib>
#include <iostream>
#include <Windows.h>
#include <bass.h>
#include <assert.h>
#include <RhythmMath.hpp>
#include <chrono>

using namespace std;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool InitAudioSystem();
void Update();

HWND g_mainHWnd;
HMUSIC g_hMusic;
HMUSIC g_hTickSound;

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
  if (!InitAudioSystem())
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

    InvalidateRect(g_mainHWnd, NULL, FALSE);
  }

  BASS_StreamFree(g_hMusic);
  BASS_Free();
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
  case WM_PAINT:
    Update();
    break;
  case WM_CHAR:
    if (wParam == 'a')
    {
      BASS_ChannelPlay(g_hMusic, FALSE);

      BASS_ChannelPlay(g_hTickSound, TRUE);
    }
    break;
  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

bool InitAudioSystem()
{
  // Check the correct BASS was loaded
  if (HIWORD(BASS_GetVersion()) != BASSVERSION)
  {
    MessageBox(0, "An incorrect version of BASS DLL was loaded", 0, MB_ICONERROR);
    return false;
  }

  BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, 1);

  if (!BASS_Init(-1, 44100, 0, g_mainHWnd, NULL))
  {
    return 0;
  }

  g_hMusic = BASS_StreamCreateFile(FALSE, TEXT("mxm.ogg"), 0, 0, 0);  

  g_hTickSound = BASS_StreamCreateFile(FALSE, TEXT("tick.wav"), 0, 0, 0);

  BASS_SetVolume(0.9f);

  BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, 1);
  //BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, 2);

  return true;
}

void Update()
{  
  static char strBytePos[10000] = "\0";

  BASS_CHANNELINFO bass_channelInfo;
  BASS_ChannelGetInfo(g_hMusic, &bass_channelInfo);
  
  DWORD frequency = bass_channelInfo.freq;
  QWORD currentSample = BASS_ChannelGetPosition(g_hMusic, BASS_POS_BYTE);

  const double CurrentBPM = 161;
  const double secPerMeasure = 60 * 4 / CurrentBPM;
  const double tickInterval = 60 * 4 / CurrentBPM / 4;

  static double prevTickTime = 0.0;
  static double nextTickTime = tickInterval;
  const double currentTime = BASS_ChannelBytes2Seconds(g_hMusic, BASS_ChannelGetPosition(g_hMusic, BASS_POS_BYTE));

  if (currentTime >= nextTickTime)
  {
    BASS_ChannelPlay(g_hTickSound, TRUE);

    prevTickTime = nextTickTime;
    nextTickTime = RhythmEngine::GetNextIntervalTime(currentTime, tickInterval);
  }

  // Draw
  static HDC hdc, backMemDC;
  static HBITMAP hBitmap, OldBitmap;
  PAINTSTRUCT ps;

  hdc = BeginPaint(g_mainHWnd, &ps);

  RECT windowRT;
  GetClientRect(g_mainHWnd, &windowRT);

  backMemDC = CreateCompatibleDC(hdc);
  hBitmap = CreateCompatibleBitmap(hdc, windowRT.right, windowRT.bottom);
  OldBitmap = (HBITMAP)SelectObject(backMemDC, hBitmap);

  FillRect(backMemDC, &windowRT, (HBRUSH)GetStockObject(WHITE_BRUSH));

  sprintf(strBytePos, "BPM : %lf | Sec Per Measure %lf | Tick Interval %lf", CurrentBPM, secPerMeasure, tickInterval);
  TextOutA(backMemDC, 0, 5, strBytePos, strlen(strBytePos));

  sprintf(strBytePos, "Frequency : %lu", frequency);
  TextOutA(backMemDC, 0, 25, strBytePos, strlen(strBytePos));

  sprintf(strBytePos, "Current Sample : %llu", currentSample);
  TextOutA(backMemDC, 0, 45, strBytePos, strlen(strBytePos));

  sprintf(strBytePos, "Milliseconds : %lu", (DWORD)(BASS_ChannelBytes2Seconds(g_hMusic, BASS_ChannelGetPosition(g_hMusic, BASS_POS_BYTE)) * 1000));
  TextOutA(backMemDC, 0, 65, strBytePos, strlen(strBytePos));

  sprintf(strBytePos, "Seconds : %lf", BASS_ChannelBytes2Seconds(g_hMusic, BASS_ChannelGetPosition(g_hMusic, BASS_POS_BYTE)));
  TextOutA(backMemDC, 0, 85, strBytePos, strlen(strBytePos));

  sprintf(strBytePos, "PrevTickTime : %lf | NextTickTime : %lf", prevTickTime, nextTickTime);
  TextOutA(backMemDC, 0, 105, strBytePos, strlen(strBytePos));

  BitBlt(hdc, 0, 0, windowRT.right, windowRT.bottom, backMemDC, 0, 0, SRCCOPY);

  DeleteObject(SelectObject(backMemDC, OldBitmap));
  DeleteDC(backMemDC);

  EndPaint(g_mainHWnd, &ps);
}