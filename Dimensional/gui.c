#include <wchar.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "game.h"
#include "gui.h"
#include "structure_data.h"

#define TICK_RATE_HTZ 60
#define TICK_RATE_MS (1000 / TICK_RATE_HTZ)

static HWND hwnd_main = NULL;
static const wchar_t* WINDOW_CLASS_NAME = L"DimensionalWindowClass";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);


uint64_t start_time = 0;

void get_window_size(HWND hwnd, int* width, int* height) {
	RECT rect;
	GetClientRect(hwnd_main, &rect);
	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
}

void gui_init(HINSTANCE hInstance, int nCmdShow) {
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WINDOW_CLASS_NAME;

	RegisterClass(&wc);
	hwnd_main = CreateWindowEx(
		0,
		WINDOW_CLASS_NAME,
		L"Dimensional GUI",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, hInstance, NULL
	);

	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);

	start_time = GetTickCount64();
	SetTimer(hwnd_main, 1, TICK_RATE_MS, NULL);

	ShowWindow(hwnd_main, nCmdShow);
	UpdateWindow(hwnd_main);
}

void gui_run() {
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void format_resource_amount(wchar_t* dest, size_t size, const resource_t* r) {
	swprintf(dest, size, L"%uE%u", r->amount_mantissa, r->amount_exponent1);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_PAINT: {
		printf("WM_PAINT\n");
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		
		//for (uint16_t i = 0; i < game_state.resource_count; i++) {
		//	const resource_t* r = &game_state.resources[i];
		//	wchar_t buffer[64];
		//	format_resource_amount(buffer, sizeof(buffer) / sizeof(wchar_t), r);

		//	wchar_t output[128];
		//	swprintf(output, sizeof(output) / sizeof(wchar_t), L"%hs: %ls", r->name, buffer);

		//	TextOutW(hdc, 20, 20 + i * 20, output, lstrlenW(output));
		//}
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_TIMER:
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_SIZE:

		return 0;

	case WM_DESTROY:
		FreeConsole();
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}