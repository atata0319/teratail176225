#define UNICODE

#include <Windows.h>
#include <Windowsx.h> // GET_X_LPARAM と GET_Y_LPARAM 用

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WindowMouseProc(HWND, UINT, WPARAM, LPARAM); // 透明ウィンドウ用プロシージャ
LRESULT CALLBACK WindowDestroyProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hGlobalInstance;

const wchar_t CLASS_NAME[] = L"CLASS NAME";
const wchar_t MOUSE_CLASS_NAME[] = L"MOUSE CLASS NAME";
const wchar_t DESTROY_CLASS_NAME[] = L"DESTROY CONTROL CLASS NAME";

HWND mhwnd;
HWND mmouse;
bool mshowdialog = false;

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

	mhwnd = CreateWindowEx(
		WS_EX_LAYERED, CLASS_NAME, NULL, WS_POPUP | WS_BORDER,
		100, 100, 960, 540,
		NULL, NULL, hInstance, NULL
	);

	SetLayeredWindowAttributes(mhwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

	WNDCLASSEX wc2 = {
		sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, WindowMouseProc,
		0, 0, hInstance,
		NULL, (HCURSOR)LoadCursor(NULL,IDC_ARROW), (HBRUSH)GetStockObject(WHITE_BRUSH),
		NULL, MOUSE_CLASS_NAME, NULL
	};

	RegisterClassEx(&wc2);

	mmouse = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_NOACTIVATE, MOUSE_CLASS_NAME, NULL, WS_POPUP,
		100, 100, 960, 540,
		NULL, NULL, hInstance, NULL
	);

	// 以下、不要でした。
	// SetLayeredWindowAttributes(mmouse, 0, 1, LWA_ALPHA);

	ShowWindow(mhwnd, nCmdShow);
	SetWindowPos(mmouse, mhwnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

	UpdateWindow(mmouse);
	UpdateWindow(mhwnd);

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

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rc = { 50, 50, 910, 530 };

		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));

		EndPaint(hwnd, &ps);
	}
	break;

	case WM_LBUTTONDOWN:
		PostMessage(mhwnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL);
		return 0;

	case WM_APP + WM_LBUTTONDOWN - WM_MOUSEFIRST:
	{
		if (mshowdialog) {
			FlashWindow(hwnd, TRUE);
			MessageBeep(MB_OK);
			break;
		}
		mshowdialog = true;
		if (MessageBox(hwnd, L"Quit?", L"WM_LBUTTONDOWN", MB_OKCANCEL) == IDOK) {
			DestroyWindow(mmouse);
			DestroyWindow(mhwnd);
		}
		mshowdialog = false;
	}
	return 0;

	case WM_MOVE:
	case WM_SIZE:
	{
		RECT rc;
		GetWindowRect(mhwnd, &rc);
		SetWindowPos(mmouse, mhwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOACTIVATE);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowMouseProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
	{
		// マウス位置をメインウィンドウのクライアント座標に変換して送信。
		POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		MapWindowPoints(hwnd, mhwnd, &point, 1);
		return SendMessage(mhwnd, WM_APP + uMsg - WM_MOUSEFIRST, wParam, MAKELPARAM(point.x, point.y));
	}
	else if (uMsg == WM_MOUSEACTIVATE)
	{
		// WS_EX_NOACTIVATE だけでは ACTIVATE してしまうので対策している。
		return MA_NOACTIVATE;
	}
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK WindowDestroyProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_LBUTTONUP:
		DestroyWindow(mmouse);
		DestroyWindow(mhwnd);
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
