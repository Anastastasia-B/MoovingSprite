#include <Windows.h>

RECT actorRC{ 0, 0, 50, 50 };

RECT _windowRC{ 0, 0, 600, 600 };

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
	HBITMAP memBM = CreateCompatibleBitmap(hdc, _windowRC.right - _windowRC.left, _windowRC.bottom - _windowRC.top);
	SelectObject(memDC, memBM);

	HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(memDC, &_windowRC, hbrBkGnd);

	HBRUSH hbrActor = CreateSolidBrush(RGB(0, 99, 71));
	FillRect(memDC, &actorRC, hbrActor);

	BitBlt(hdc, 0, 0, _windowRC.right - _windowRC.left, _windowRC.bottom - _windowRC.top, memDC, 0, 0, SRCCOPY);
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

	AdjustWindowRect(&_windowRC, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		L"Mooving Sprite",
		WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE,
		(GetSystemMetrics(SM_CXSCREEN) - _windowRC.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - _windowRC.bottom) / 2,
		_windowRC.right, _windowRC.bottom, nullptr, nullptr, nullptr, wc.hInstance);

	if (hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	HDC hdc = GetDC(hwnd);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		WinShow(hdc);
	}

	return static_cast<int> (msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_MOUSEWHEEL:
		{
			int move = GET_WHEEL_DELTA_WPARAM(wParam) / 20;
			if (isShift()) {
				actorRC.left += move;
				actorRC.right += move;
			}
			else {
				actorRC.top += move;
				actorRC.bottom += move;
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
