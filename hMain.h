#ifndef HMAIN_H
#define HMAIN_H

#include <iostream>
#include <Windows.h>

#include "hD3D9.h"

#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")


extern int windowWidth; // Game window width
extern int windowHeight; // Game window height
extern char lWindowName[256]; // Overlay window name (our overlay window name)
extern HWND hWnd; // Handle to our window
extern char tWindowName[256]; // Target window name (our game window name)
extern HWND TargetWnd; // Our target window handle
extern RECT WindowRect, ClientRect; // Rectangles, coordinates
extern MSG Message; // Queue message..

LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

#endif