#include "hD3d9.h"

CHackProcess fProcess;
int NumOfPlayers = 16;
#define PI 3.14159265


IDirect3D9Ex* dx_Object = NULL;
IDirect3DDevice9Ex* dx_Device = NULL;
D3DPRESENT_PARAMETERS dx_Param; 
ID3DXLine* dx_Line;
ID3DXFont* dx_Font = 0;

/*
We require to initialize the D3D drawing, so we require hWnd. Windows identifies each form or application by assigning it a handle or also known as hWnd.
*/
int D3D9Init(HWND hWnd)
{
	/*
	We need to check to see if we can create an IDirect3D9Ex object and return an interface to it. Why is D3D_SDK_VERSION passed? Because we will need to ensure that the header files used in the compiled application match the version of the installed runtime DLLs. Why are we passing the object to dx_Object? Because we are creating an IDirect3D9Ex object, and we need to store it somewhere. If it fails, the app crashes (the DLL), and if it passes, it continues, simple huh?
	*/
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &dx_Object)))
	{
		exit(1);
	}

	/*
	We created the dx_Param earlier, it is a D3DPRESENT_PARAMETERS structure. It contains many variables you can modify but in this source we are only modifying these variables.

	BackBufferFormat (D3DFORMAT) is the buffer that is drawn off-screen and will be switched with the front buffer at the next frame. This is considered double buffering, which is what you need to do in GDI to ensure that it does not flicker. But GDI will still flicker because it is "slow" you could persay.

	D3DFMT_A8R8G8B8	(Value: 21) is an 32-bit ARGB pixel format with alpha, using 8 bits per channel.
	*/
	dx_Param.BackBufferFormat = D3DFMT_A8R8G8B8;

	dx_Param.BackBufferWidth = windowWidth;
	dx_Param.BackBufferHeight = windowHeight;
	dx_Param.EnableAutoDepthStencil = TRUE;
	dx_Param.AutoDepthStencilFormat = D3DFMT_D16;

	/*
	hDeviceWindow (HWND) is the form or application that determines the location and size of the back buffer on the screen.
	*/
	dx_Param.hDeviceWindow = hWnd;

	/*
	MultiSampleQuality (DWORD) is the quality level. Technically speaking DEFAULT_QUALITY is zero which also is kind of funny because zero is the lowest MultiSampleQuality. Why are we setting this? Well this is all GPU related, and microsoft is extremely vauge about this, so we will just leave this as zero.
	*/
	dx_Param.MultiSampleQuality = DEFAULT_QUALITY;

	/*
	SwapEffect (D3DSWAPEFFECT) is how the front and back buffer are to be swapped. When we disregard this, we can do multi sampling (above).
	*/
	dx_Param.SwapEffect = D3DSWAPEFFECT_DISCARD;

	/*
	Windowed (BOOL) is basically asking you if the form or application is running windowed or fullscreen. True is windowed. False is fullscreen.
	*/
	dx_Param.Windowed = TRUE;


	/*
	We need to see if we can create a device to REPRESENT the display adapter.
	D3DADAPTER_DEFAULT (UNIT) is the always the primary display adapter.
	D3DDEVTYPE_HAL (D3DDEVTYPE) (value: 1) is hardware rasterization. Shading is done with software, hardware, or mixed transform and lighting.
	hWnd (HWND) is the form we will create the device in mind with. Something I noticed was that this can be null if we set the hDeviceWindow to a non-null value, which it is. So I changed it to a null.
	D3DCREATE_HARDWARE_VERTEXPROCESSING (DWORD) specifies hardware vertex processing.
	dx_Param (D3DPRESENT_PARAMTERS*) describe the presentation parameters for the device to be created.
	0 (D3DDISPLAYMODEEX*) is only used when the device is set to fullscreen, which it is not for this source, but it basically is the properties of a display mode (size, width, height, refresh rate, format, scanlineordering).
	dx_Device (IDirect3DDevice9Ex**) is the address of a pointer to the returned IDirect3DDevice9Ex, which represents the created device.
	*/
	if (FAILED(dx_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dx_Param, 0, &dx_Device)))
	{
		exit(1);
	}


	if (!dx_Line)
		D3DXCreateLine(dx_Device, &dx_Line);

	/*
	D3DXCreateFont creates a font object for a device and font.

	D3DXCreateFont(device, h, w, weight, miplevels, italic, charset, OutputPrecision, quality, pitchandfamily, pfaceanme, *ppfont)

	dx_Device (LPDIRECT3DDEVICE9) is the device we will be creating a font for.
	24 (INT) is the height of the characters in logical units.
	0 (UINT) is the width of the characters in logical units.
	FW_REGULAR (UNIT) is the typeface weight.
	0 (MipLevels) is the number of mipmap levels. MipMaps (not miplevels) are pre-calculated, optimized collections of images that accompany a main texture, intended to increase rendering speed and reduce aliasing artifacts
	false (BOOL) is if the italic font is true or not. In this case it is false.
	DEFAULT_CHARSET ( DWORD) is the character set of the font.
	OUT_CHARACTER_PRECIS (DWORD) specifies how Windows should attempt to match the desired font sizes and characteristics with actual fonts. In this case we are not using this feature.
	ANTIALIASED_QUALITY (DWORD) specifies how Windows should match the desired font with a real font. In this case we are always antialiasing if the font supports it and the size of the font is not too small or too large.
	DEFAULT_PITCH (DWORD) is the pitch and family index.
	Verdana (LPCTSTR) is the string containing the typeface name (font style).
	dx_Font (LPD3DXFONT*) returns a pointer to an ID3DXFont interface, representing the created font object.
	*/
	D3DXCreateFont(dx_Device, 18, 0, FW_LIGHT, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial", &dx_Font);

	return 0;
}

typedef struct // ESP
{
	float rawflMatrix[5][4];
}WorldToScreenMatrix_t;

struct MyPlayer_t
{
	DWORD CLocalPlayer;
	int Team;
	int Health;
	WorldToScreenMatrix_t WorldToScreenMatrix;  // ESP
	float Position[3];
	float CurrentAng[3];
	int Flags;
	int CrosshairID;
	int flickerCheck, flickerCheck2, flickerCheck3, flickerCheck4; 

	void ReadInformation()
	{
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + dw_playerbase), &CLocalPlayer, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_health_offset), &Health, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_team_offset), &Team, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_position_offset), &Position, sizeof(float[3]), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_flags_offset), &Flags, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_crosshair_offset), &CrosshairID, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_mouse_x), &CurrentAng, sizeof(float[3]), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_antiFlick), &flickerCheck, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_antiFlick2), &flickerCheck2, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_antiFlick3), &flickerCheck3, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_antiFlick4), &flickerCheck4, sizeof(int), 0);

		// VMATRIX
		if ((flickerCheck == 1) && (flickerCheck2 == 0) && (flickerCheck3 == 0) && (flickerCheck4 == 0))
		{
			ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_vmatrix), &WorldToScreenMatrix, sizeof(WorldToScreenMatrix), 0);
		}

	}
}MyPlayer;

struct TargetList_t
{

	float Distance;
	float AimAngle[3];

	// Constructor
	TargetList_t() {	}

	// Grab the distance and aim?
	TargetList_t(float MyCoords[], float EnemyCoords[], float aimAngle[])
	{
		// Find out how far the enemy is
		Distance = Get3dDistance(MyCoords, EnemyCoords);

		AimAngle[0] = aimAngle[0];
		AimAngle[1] = aimAngle[1];
		AimAngle[2] = aimAngle[2];
	}


}TargetList;

struct PlayerList_t
{
	DWORD CBaseEntity;
	int Team;
	int Health;
	int Dorm;
	int Flags;
	DWORD Bones;
	float Position[3];
	float BonePosition[3];
	float AimAngle[3]; // ?
	float Distance; // ?

	void ReadInformation(int player, int iBone)
	{
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + dw_entitybase + (player * dw_entity_loop_distance)), &CBaseEntity, sizeof(DWORD), 0); // CBaseEntity
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_health_offset), &Health, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_team_offset), &Team, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_dormant_offset), &Dorm, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_position_offset), &Position, sizeof(float[3]), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_flags_offset), &Flags, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_bone_offset), &Bones, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PVOID)(Bones + 0x30 * iBone + 0xC), &BonePosition[0], sizeof(float), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PVOID)(Bones + 0x30 * iBone + 0x1C), &BonePosition[1], sizeof(float), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PVOID)(Bones + 0x30 * iBone + 0x2C), &BonePosition[2], sizeof(float), 0);
		BonePosition[2] -= 60;
	}
}PlayerList[32];

struct CompareTargetArray
{
	bool operator () (TargetList_t & lhs, TargetList_t & rhs)
	{
		int x = lhs.Distance;
		int y = rhs.Distance;
		return x < y;
	}
};

float Get3dDistance(float * myCoords, float * enemyCoords)
{
	return sqrt(
		pow(double(enemyCoords[0] - myCoords[0]), 2.0) +
		pow(double(enemyCoords[1] - myCoords[1]), 2.0) +
		pow(double(enemyCoords[2] - myCoords[2]), 2.0));

}

void CalcAngle(float *src, float *dst, float *angles)
{
	double delta[3] = { (src[0] - dst[0]), (src[1] - dst[1]), (src[2] - dst[2]) };
	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
	angles[0] = atan(delta[2] / hyp) * 180.0 / PI; //angles[0] = (float) (asinf(delta[2]/hyp) * 57.295779513082f);
	angles[1] = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	angles[2] = 0.0f;

	if (delta[0] >= 0.0)
	{
		angles[1] += 180.0f;
	}
}

bool WorldToScreen(float * from, float * to)
{
	float w = 0.0f;
	float flMatrix[4][4];
	
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			flMatrix[i][j] = MyPlayer.WorldToScreenMatrix.rawflMatrix[i][j];
		}
	}

	to[0] = flMatrix[0][0] * from[0] + flMatrix[0][1] * from[1] + flMatrix[0][2] * from[2] + flMatrix[0][3];
	to[1] = flMatrix[1][0] * from[0] + flMatrix[1][1] * from[1] + flMatrix[1][2] * from[2] + flMatrix[1][3];
	w = flMatrix[3][0] * from[0] + flMatrix[3][1] * from[1] + flMatrix[3][2] * from[2] + flMatrix[3][3];

	if (w < 0.01f)
		return false;

	float invw = 1.0f / w;
	to[0] *= invw;
	to[1] *= invw;
	
	float x = windowWidth / 2;
	float y = windowHeight / 2;

	x += 0.5 * to[0] * windowWidth + 0.5;
	y -= 0.5 * to[1] * windowHeight + 0.5;

	to[0] = x + WindowRect.left;
	to[1] = y + WindowRect.top;

	return true;
}

void GetNumberOfPlayers()
{
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_numofplayers), &NumOfPlayers, sizeof(int), 0);
}

void ESP(int x, int y, int distance, int hp, int team, int dorm)
{
	char text[64];
	int width = 18100 / distance;
	int height = 36000 / distance;
	int x1 = x - (width / 2);
	int y1 = y - height;

	if (team == 3)
	{
		DrawBox(x1, y1, width, height, 2, 0, 0, 255, 255);
	}
	else if (team == 2)
	{
		DrawBox(x1, y1, width, height, 2, 255, 0, 0, 255);
	}
	else if (team == 1)
	{
		DrawBox(x1, y1, width, height, 2, 0, 255, 0, 255);
	}

	if (menuEnabled[2])
	{
		DrawHealthBarBack(x1, y1, (hp / 1.5) + 2, 6, 200);
		DrawHealthBar(x1 + 1, y1 + 1, hp / 1.5, 4, 0, 255, 0, 200);
	}

	if (menuEnabled[3])
	{
		sprintf(text, "%d", distance);
		DrawString(text, x1, y1 - 20, 180, 180, 180, dx_Font);
	}
	
}

void Aimbot()
{

	TargetList_t * TargetList = new TargetList_t[NumOfPlayers];
	int targetLoop = 0;
	float EnemyXY[3];

	for (int i = 1; i < NumOfPlayers; i++)
	{

		PlayerList[i].ReadInformation(i, 14);

		if (PlayerList[i].Health < 2)
		{
			continue;
		}

		if ((PlayerList[i].Dorm == 1) || (PlayerList[i].Dorm == -65535))
		{
		}
		else
		{

			if ((WorldToScreen(PlayerList[i].BonePosition, EnemyXY)) && menuEnabled[1]) // W2S and Is ESP enabled?
			{

				int distanceX = Get3dDistance(MyPlayer.Position, PlayerList[i].Position); // My player to enemy player distance

				if (distanceX > 0)
				{
					ESP(EnemyXY[0] - WindowRect.left, EnemyXY[1] - WindowRect.top, distanceX, PlayerList[i].Health, PlayerList[i].Team, PlayerList[i].Dorm);
				}
			}

			if (!menuEnabled[0])
			{
				continue;
			}

			CalcAngle(MyPlayer.Position, PlayerList[i].BonePosition, PlayerList[i].AimAngle);

			if (PlayerList[i].Team != MyPlayer.Team)
			{

				TargetList[targetLoop] = TargetList_t(MyPlayer.Position, PlayerList[i].BonePosition, PlayerList[i].AimAngle);

				targetLoop++;
			}

		}
	}

	if ((targetLoop > 0) && (menuEnabled[0]) && (GetAsyncKeyState(0x58))) // Do we have enemies and is aimbot on? Is key pressed ('X' == 0x58)?
	{

		// Potential enemy list start, end, and then comparitive (basically sort by distance)
		std::sort(TargetList, TargetList + targetLoop, CompareTargetArray());

		WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_mouse_x), TargetList[0].AimAngle, 12, 0);
	}

	targetLoop = 0;
	delete[] TargetList;


}

void keyState()
{

	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		showMenu = !showMenu;
	}
	else if (showMenu == false)
	{
		// Don't go to the other if statements
	}
	else if (GetAsyncKeyState(VK_UP) & 1)
	{
		if (selectedItem > 0)
		{
			selectedItem--;
		}
	}
	else if (GetAsyncKeyState(VK_DOWN) & 1)
	{
		if (selectedItem < (maxItems - 1))
		{
			selectedItem++;
		}
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 1)
	{
		menuEnabled[selectedItem] = !menuEnabled[selectedItem];
	}

	if (showMenu)
	{

		for (size_t i = 0; i < maxItems; i++)
		{
			char text[64];
			if (menuEnabled[i])
			{
				if (selectedItem == i)
				{
					sprintf(text, "%s: ON", menuItems[i]);
					DrawString(text, 5, 10 + (20 * i), 0, 150, 0, dx_Font);
				}
				else
				{
					sprintf(text, "%s: ON", menuItems[i]);
					DrawString(text, 5, 10 + (20 * i), 0, 255, 0, dx_Font);
				}
			}
			else
			{
				if (selectedItem == i)
				{
					sprintf(text, "%s: OFF", menuItems[i]);
					DrawString(text, 5, 10 + (20 * i), 150, 0, 0, dx_Font);
				}
				else
				{
					sprintf(text, "%s: OFF", menuItems[i]);
					DrawString(text, 5, 10 + (20 * i), 255, 0, 0, dx_Font);
				}

			}
		}
	}

}

int Render()
{
	dx_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	dx_Device->BeginScene();

	if (TargetWnd == GetForegroundWindow())
	{
		MyPlayer.ReadInformation();
		keyState();
		Aimbot();

		// Crosshair
		DrawFilled(windowWidth / 2 - 22, windowHeight / 2, 44, 1, 240, 240, 250, 255);
		DrawFilled(windowWidth / 2, windowHeight / 2 - 22, 1, 44, 240, 240, 250, 255);
	}

	dx_Device->EndScene();
	dx_Device->PresentEx(0, 0, 0, 0, 0);

	return 0;
}


