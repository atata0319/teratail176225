#define UNICODE

#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WindowDestroyProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hGlobalInstance;

const wchar_t CLASS_NAME[] = L"CLASS NAME";
const wchar_t DESTROY_CLASS_NAME[] = L"DESTROY CONTROL CLASS NAME";

HWND mhwnd;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR nCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdLine);
	hGlobalInstance = hInstance;

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, WindowProc,
		0, 0, hInstance,
		NULL, (HCURSOR)LoadCursor(NULL,IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH),
		NULL, CLASS_NAME, NULL
	};

	RegisterClassEx(&wc);

	HWND hwnd = CreateWindowEx(
		WS_EX_LAYERED, CLASS_NAME, NULL, WS_POPUP | WS_BORDER,
		100, 100, 960, 540,
		NULL, NULL, hInstance, NULL
	);

	mhwnd = hwnd;

	SetLayeredWindowAttributes(hwnd, 0x0, 0, LWA_COLORKEY);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
	{
		WNDCLASSEX wdc = {
			sizeof(WNDCLASSEX), NULL, WindowDestroyProc,
			0, 0, hGlobalInstance,
			NULL, (HCURSOR)LoadCursor(NULL,IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH),
			NULL, DESTROY_CLASS_NAME, NULL
		};
		RegisterClassEx(&wdc);
		HWND dwnd = CreateWindowEx(
			0, DESTROY_CLASS_NAME, NULL, WS_CHILD,
			815, 30, 105, 30,
			hwnd, NULL, hGlobalInstance, NULL
		);

		ShowWindow(dwnd, SW_SHOW);
		UpdateWindow(dwnd);
	}
	break;

	case WM_LBUTTONDOWN:
		PostMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rc = { 50, 50, 910, 530 };

		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));

		EndPaint(hwnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowDestroyProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_LBUTTONUP:
		SendMessage(mhwnd, WM_DESTROY, 0, 0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rc = { 5, 5, 100, 25 };

		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));

		EndPaint(hwnd, &ps);
	}
	break;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
