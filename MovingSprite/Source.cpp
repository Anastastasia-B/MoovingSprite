#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")

RECT actorRC{ 200, 200, 250, 250 };

const int buttonHeight = 40;

RECT sceneRC;
RECT windowRC{ 0, 0, 600, 500 };
HDC hdc;

int moveByButton = 3;

const int ANIM_BUTTON = 1234;
const int SPRITE_BUTTON = 1235;

bool animMode = false;
bool spriteMode = false;
HWND animButton;
HWND spriteButton;

POINT speed{ 3, 2 };

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool isShift()
{
	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		return TRUE;
	return FALSE;
}

void ShowSprite(HDC hdc)
{
	ULONG_PTR token;
	GdiplusStartupInput input;
	GdiplusStartup(&token, &input, 0);

	Bitmap bitmap(L"actor.png");
	UINT w = bitmap.GetWidth(), h = bitmap.GetHeight();

	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP memBM = CreateCompatibleBitmap(hdc, sceneRC.right - sceneRC.left, sceneRC.bottom - sceneRC.top);
	SelectObject(memDC, memBM);

	HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(memDC, &sceneRC, hbrBkGnd);

	HDC imgDC = CreateCompatibleDC(hdc);
	HBITMAP imgBM;
	bitmap.GetHBITMAP(GetSysColor(COLOR_WINDOW), &imgBM);

	HBITMAP hPrevBmp = (HBITMAP)SelectObject(imgDC, imgBM);
	StretchBlt(memDC, actorRC.left, actorRC.top, actorRC.right - actorRC.left, actorRC.bottom - actorRC.top, imgDC, 0, 0, w, h, SRCCOPY);

	BitBlt(hdc, 0, buttonHeight, sceneRC.right - sceneRC.left, sceneRC.bottom - sceneRC.top, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, hPrevBmp);

	DeleteObject(hbrBkGnd);
	DeleteObject(memBM);
	DeleteDC(memDC);
}

void ShowRect(HDC hdc)
{
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP memBM = CreateCompatibleBitmap(hdc, sceneRC.right - sceneRC.left, sceneRC.bottom - sceneRC.top);
	SelectObject(memDC, memBM);

	HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(memDC, &sceneRC, hbrBkGnd);

	HBRUSH hbrActor = CreateSolidBrush(RGB(200, 50, 100));
	FillRect(memDC, &actorRC, hbrActor);

	BitBlt(hdc, 0, buttonHeight, sceneRC.right - sceneRC.left, sceneRC.bottom - sceneRC.top, memDC, 0, 0, SRCCOPY);

	DeleteObject(hbrBkGnd);
	DeleteObject(hbrActor);
	DeleteObject(memBM);
	DeleteDC(memDC);
}

void moveHor(int move)
{
	if ((move > 0 && actorRC.right < sceneRC.right) || (move < 0 && actorRC.left > sceneRC.left))
	{
		actorRC.left += move;
		actorRC.right += move;
	}
}

void moveVert(int move)
{
	if ((move > 0 && actorRC.bottom < sceneRC.bottom) || (move < 0 && actorRC.top > sceneRC.top))
	{
		actorRC.top += move;
		actorRC.bottom += move;
	}
}

void animate()
{
	if (actorRC.bottom > sceneRC.bottom || actorRC.top < sceneRC.top)
		speed.y *= -1;
	if (actorRC.right > sceneRC.right || actorRC.left < sceneRC.left)
		speed.x *= -1;

	actorRC.left += speed.x;
	actorRC.right += speed.x;
	actorRC.top += speed.y;
	actorRC.bottom += speed.y;
}

void Update()
{
	if (animMode)
	{
		animate();
	}
	else
	{
		if ((GetKeyState(VK_LEFT) & 0x8000) != 0)
			moveHor(-moveByButton);
		if ((GetKeyState(VK_RIGHT) & 0x8000) != 0)
			moveHor(moveByButton);
		if ((GetKeyState(VK_UP) & 0x8000) != 0)
			moveVert(-moveByButton);
		if ((GetKeyState(VK_DOWN) & 0x8000) != 0)
			moveVert(moveByButton);
	}

	if (spriteMode)
		ShowSprite(hdc);
	else
		ShowRect(hdc);
}

void GetSceneRect(HWND hwnd)
{
	GetClientRect(hwnd, &sceneRC);
	sceneRC.bottom -= buttonHeight;
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	MSG msg{};
	HWND hwnd{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"MoovingSpriteClass";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	AdjustWindowRect(&windowRC, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		L"Mooving Sprite",
		WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - windowRC.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - windowRC.bottom) / 2,
		windowRC.right, windowRC.right, nullptr, nullptr, nullptr, wc.hInstance);

	if (hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	hdc = GetDC(hwnd);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (true)
	{
		if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			Update();

		Sleep(3);
	}

	return static_cast<int> (msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
			GetSceneRect(hWnd);
			animButton = CreateWindow(
				L"BUTTON",
				L"Show Animation",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				0,	0,	buttonHeight, sceneRC.right / 2,
				hWnd,
				reinterpret_cast<HMENU>(ANIM_BUTTON),
				nullptr,
				nullptr
			);
			spriteButton = CreateWindow(
				L"BUTTON",
				L"To Sprite",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				sceneRC.right / 2, 0, buttonHeight, sceneRC.right / 2,
				hWnd,
				reinterpret_cast<HMENU>(SPRITE_BUTTON),
				nullptr,
				nullptr
			);
		}
		return 0;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case ANIM_BUTTON:
			{
				animMode = !animMode;
				SendMessage(animButton, WM_SETTEXT, 0, (LPARAM)(animMode ? L"Return to Manual Control" : L"Show Animation"));
			}
			break;
			case SPRITE_BUTTON:
			{
				spriteMode = !spriteMode;
				SendMessage(spriteButton, WM_SETTEXT, 0, (LPARAM)(spriteMode ? L"To Sprite" : L"To Rectangle"));
			}
			break;
			}
		}
		return 0;

		case WM_SIZE: {
			GetSceneRect(hWnd);
			SetWindowPos(animButton, nullptr, 0, 0, LOWORD(lParam) / 2, buttonHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
			SetWindowPos(spriteButton, nullptr, LOWORD(lParam) / 2, 0, LOWORD(lParam) / 2, buttonHeight, SWP_NOZORDER | SWP_NOOWNERZORDER);
		}
		return 0;

		case WM_MOUSEWHEEL:
		{
			if (!animMode)
			{
				int move = GET_WHEEL_DELTA_WPARAM(wParam) / 20;
				if (isShift()) {
					moveHor(move);
				}
				else {
					moveVert(move);
				}
			}
		}
		return 0;

		case WM_DESTROY:
		{
			PostQuitMessage(EXIT_SUCCESS);
		}
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
