#include <Windows.h>

RECT actorRC{ 200, 200, 250, 250 };

int winWidth = 600;
int winHeight = 600;

RECT clienRC;
RECT windowRC{ 0, 0, winWidth, winHeight };
HDC hdc;

float moveByButton = 5;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool isShift()
{
	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		return TRUE;
	return FALSE;
}

void WinShow(HDC hdc)
{
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP memBM = CreateCompatibleBitmap(hdc, windowRC.right - windowRC.left, windowRC.bottom - windowRC.top);
	SelectObject(memDC, memBM);

	HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(memDC, &windowRC, hbrBkGnd);

	HBRUSH hbrActor = CreateSolidBrush(RGB(0, 99, 71));
	FillRect(memDC, &actorRC, hbrActor);

	BitBlt(hdc, 0, 0, windowRC.right - windowRC.left, windowRC.bottom - windowRC.top, memDC, 0, 0, SRCCOPY);
}

void moveHor(int move)
{
	if ((move > 0 && actorRC.right < clienRC.right) || (move < 0 && actorRC.left > clienRC.left))
	{
		actorRC.left += move;
		actorRC.right += move;
	}
}

void moveVert(int move)
{
	if ((move > 0 && actorRC.bottom < clienRC.bottom) || (move < 0 && actorRC.top > clienRC.top))
	{
		actorRC.top += move;
		actorRC.bottom += move;
	}
}

void Update()
{
	if ((GetKeyState(VK_LEFT) & 0x8000) != 0)
		moveHor(-moveByButton);
	if ((GetKeyState(VK_RIGHT) & 0x8000) != 0)
		moveHor(moveByButton);
	if ((GetKeyState(VK_UP) & 0x8000) != 0)
		moveVert(-moveByButton);
	if ((GetKeyState(VK_DOWN) & 0x8000) != 0)
		moveVert(moveByButton);

	WinShow(hdc);
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
		WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE,
		(GetSystemMetrics(SM_CXSCREEN) - windowRC.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - windowRC.bottom) / 2,
		winWidth,
		winHeight,
		nullptr, nullptr, nullptr, wc.hInstance);

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
			GetClientRect(hWnd, &clienRC);
			break;
		}
		return 0;

		case WM_MOUSEWHEEL:
		{
			int move = GET_WHEEL_DELTA_WPARAM(wParam) / 20;
			if (isShift()) {
				moveHor(move);
			}
			else {
				moveVert(move);
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
