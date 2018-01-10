#ifndef HDIRECTX_H
#define HDIRECTX_H

#include "hMain.h" // WindowRect and a few others required from here
#include "hDraw.h" // We draw in d3d9.cpp so we need the methods
#include "hCore.h" // our core stuff

#include <iostream>
#include <Windows.h>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

extern IDirect3D9Ex* dx_Object;
extern IDirect3DDevice9Ex* dx_Device;
extern D3DPRESENT_PARAMETERS dx_Params;
extern ID3DXLine* dx_Line;
extern ID3DXFont* dx_Font;

int D3D9Init(HWND hWnd);

float Get3dDistance(float * myCoords, float * enemyCoords);
void CalcAngle(float *src, float *dst, float *angles);
bool WorldToScreen(float * from, float * to);
void GetNumberOfPlayers();
void ESP(int x, int y, int distance, int hp, int team, int dorm);
void Aimbot();
void keyState();
int Render();

extern int NumOfPlayers;
#endif