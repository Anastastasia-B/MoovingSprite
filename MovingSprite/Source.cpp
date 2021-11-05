#include <Windows.h>
#include <windowsx.h>

RECT actorRC{ 200, 200, 250, 250 };

int winWidth = 600;
int winHeight = 600;

RECT sceneRC;
RECT windowRC{ 0, 0, winWidth, winHeight };
HDC hdc;

int moveByButton = 3;

const int ANIM_BUTTON = 1234;

bool anim = false;

POINT speed{ 3, 2};

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool isShift()
{
	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		return TRUE;
	return FALSE;
}

void Show(HDC hdc)
{
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP memBM = CreateCompatibleBitmap(hdc, sceneRC.right - sceneRC.left, sceneRC.bottom - sceneRC.top);
	SelectObject(memDC, memBM);

	HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(memDC, &sceneRC, hbrBkGnd);

	HBRUSH hbrActor = CreateSolidBrush(RGB(0, 99, 71));
	FillRect(memDC, &actorRC, hbrActor);

	BitBlt(hdc, 0, 60, sceneRC.right - sceneRC.left, sceneRC.bottom - sceneRC.top, memDC, 0, 0, SRCCOPY);
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

	/*if (speed.x != 0 && speed.y != 0) {
		speed.x = speed.x * 0.7;
		speed.y = speed.y * 0.7;
	}*/
}

void Update()
{
	if (anim)
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

	Show(hdc);
}

void GetDrawableRect(HWND hwnd)
{
	GetClientRect(hwnd, &sceneRC);
	sceneRC.bottom -= 60;
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
		winWidth, winHeight, nullptr, nullptr, nullptr, wc.hInstance);

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
		else {
			Update();
		}

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
			HWND animButton = CreateWindow(
				L"BUTTON",
				L"Show Animation",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				10,	10,	150, 40,
				hWnd,
				reinterpret_cast<HMENU>(ANIM_BUTTON),
				nullptr,
				nullptr
			);

			GetDrawableRect(hWnd);
		}
		return 0;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case ANIM_BUTTON:
			{
				anim = !anim;
			}
			break;
			}
		}
		return 0;

		case WM_SIZE: {
			GetDrawableRect(hWnd);
		}
		return 0;

		case WM_MOUSEWHEEL:
		{
			if (!anim)
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
