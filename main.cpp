/*
Credits
TastyHorror - putting this together, etc...
s0beit - lots of stuff
Grab - overlay
NubTIK - hprocess
Fleep - w2s, calcangle, etc
Agent Smith - he is the king :)
unknowncheats - epic site
guidedhacking - epic site
And anyone else I forgot to mention...

*/

#include "hMain.h"
#include "hCore.h" 

HWND hWnd, TargetWnd; // handles to our game and overlay
MSG Message;
RECT WindowRect, ClientRect;  // Rect, coordinates basically
int windowHeight, windowWidth; // size of window
int clientHeight = 600, clientWidth = 800; // size of window (aka client)
int borderHeight, borderWidth; // border sizes

bool showMenu = true;
int selectedItem = 0;
int maxItems = 4;
char menuItems[4][32] = { "Aimbot", "ESP", "Health", "Distance" };
bool menuEnabled[4] = { true, true, true, true };

char lWindowName[256] = "Overlay";
char TargetWndName[256] = "Counter-Strike Source";

const MARGINS pMargin = { 0, 0, clientWidth, clientHeight };

// Callback, function pointer that is called everytime a certain condition is met.
LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{

	case WM_PAINT: // we need to paint? lets paint!
		Render();
		break;
	case WM_CREATE:
		return DwmExtendFrameIntoClientArea(hWnd, &pMargin); // extension of window frame into client area
		break;
	case WM_DESTROY:
		PostQuitMessage(0); // We need to use this to exit a message loop
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam); // Making sure all messages are processed
		break;
	}
}

// C-Based framework for creating win apps
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	WNDCLASSEX OverlayWnd; // contains window class information
	OverlayWnd.cbSize = sizeof(WNDCLASSEX); // size of struct, basically checking for version or check
	OverlayWnd.style = CS_HREDRAW | CS_VREDRAW;  // Style, redraw method type
	OverlayWnd.lpfnWndProc = Proc; // Pointer to the window procedure
	OverlayWnd.cbClsExtra = 0; // window class struct extra bytes
	OverlayWnd.cbWndExtra = 0; // window instance extra bytes
	OverlayWnd.hInstance = hInstance; // handle to the instance that contains the window procedure for the class
	OverlayWnd.hIcon = LoadIcon(NULL, IDI_APPLICATION); // basic window icon set
	OverlayWnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // basic window icon set
	OverlayWnd.hCursor = LoadCursor(NULL, IDC_ARROW); // basic window cursor icon set
	OverlayWnd.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)); // handle to the class background brush
	OverlayWnd.lpszMenuName = lWindowName;
	OverlayWnd.lpszClassName = lWindowName;

	// registers a window class for the use in call to this createwindowex func
	if (!RegisterClassEx(&OverlayWnd))
	{
		exit(1);
	}

	TargetWnd = FindWindow(0, TargetWndName);


	/*
	CreateWindowEx creates an overlapped, pop-up, or child window with an extended window style.

	dwExStyle (DWORD) is the extended window style of the window being created.
	WS_EX_TOPMOST means that the window should be placed above all non-topmost windows and should stay above them, even when the window is deactivated
	WS_EX_LAYERED uses a layered window can significantly improve performance and visual effects for a window that has a complex shape, animates its shape, or wishes to use alpha blending effects.
	WS_EX_COMPOSITED paints all descendants of a window in bottom-to-top painting order using double-buffering.
	WS_EX_TRANSPARENT means that the window should not be painted until siblings beneath the window (that were created by the same thread) have been painted.

	lpClassName (LPCTSTR) is a null-terminated string or a class atom created by a previous call to the RegisterClass or RegisterClassEx function.
	lpWindowName (LPCSTR) is the window name.
	dwStyle (DWORD) is the style of the window being created.
	WS_POPUP means that the window is a pop-up window.
	x (int) is the horizontal position of the window.
	y (int) is the vertical position of the window.
	nWidth (int) is the width.
	nHeight (int) is the height.

	The last three nulls are all optional, and I wont bother mentioning them. If you are interested google CreateWindowEx.

	The dimensions for the overlay will be resized when the game is found.
	*/


	if (TargetWnd)
	{
		GetWindowRect(TargetWnd, &WindowRect);
		windowWidth = WindowRect.right - WindowRect.left;
		windowHeight = WindowRect.bottom - WindowRect.top;
		hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, lWindowName, lWindowName, WS_POPUP, 1, 1, windowWidth, windowHeight, 0, 0, 0, 0);
	}
	
	/*
	SetLayeredWindowAttributes sets the opacity and transparency color key for a layered window.
	*/
	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

	/*
	Show the layered window aka our overlay.
	*/
	ShowWindow(hWnd, SW_SHOW);

	/*
	We use our handle to our overlay and initalize our D3D adapter.
	*/
	D3D9Init(hWnd);

	bool panic = false;
	fProcess.RunProcess();

	/*
	While we are not panicking, we will be enable our hack.
	*/
	while (panic == false)
	{

		if (GetAsyncKeyState(VK_F12) & 1)
		{
			panic = true;
		}

		/*
		Dispatches incoming sent messages, checks the thread message queue for a posted message, and retrieves the message (if any exist). Messages are removed from the queue after processing due to PM_REMOVE.
		*/
		if (PeekMessage(&Message, hWnd, 0, 0, PM_REMOVE))
		{
			/*
			Translates virtual-key messages into character messages.
			*/
			TranslateMessage(&Message);

			/*
			Dispatches a message to a window procedure.
			*/
			DispatchMessage(&Message);
		}

		/*
		Set the hWnd for the game we want.
		*/
		TargetWnd = FindWindow(0, TargetWndName);

		/*
		If there is no game, quit.
		*/
		if (!TargetWnd)
		{
			exit(0);
		}

		/*
		Set the RECT using the targeted window.
		*/
		GetWindowRect(TargetWnd, &WindowRect);
		GetClientRect(TargetWnd, &ClientRect);

		windowWidth = WindowRect.right - WindowRect.left;
		windowHeight = WindowRect.bottom - WindowRect.top;

		/*
		Resize the overlay matching what is required.
		*/
		DWORD dwStyle = GetWindowLong(TargetWnd, GWL_STYLE);

		if (dwStyle & WS_BORDER)
		{
			windowHeight = WindowRect.bottom - WindowRect.top;
			windowWidth = WindowRect.right - WindowRect.left;
			clientHeight = ClientRect.bottom - ClientRect.top;
			clientWidth = ClientRect.right - ClientRect.left;
			borderHeight = (windowHeight - ClientRect.bottom);
			borderWidth = (windowWidth - ClientRect.right) / 2; //only want one side
			borderHeight -= borderWidth; //remove bottom from width, bottom is the same size as either side so we subtract that

			WindowRect.left += borderWidth;
			WindowRect.top += borderHeight;
		}

		MoveWindow(hWnd, WindowRect.left, WindowRect.top, clientWidth, clientHeight, true);

	} // End of Panic Loop

	/*
	Lets exit immediately...
	*/
	exit(0);

}