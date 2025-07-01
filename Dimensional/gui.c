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

int width = 0;
int height = 0;
uint64_t start_time = 0;

void get_window_size(HWND hwnd) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

void gui_init(HINSTANCE hInstance, int nCmdShow) {
	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, L"RegisterClass failed", L"Error", MB_ICONERROR);
		return;
	}
	hwnd_main = CreateWindowEx(
		0,
		WINDOW_CLASS_NAME,
		L"Dimensional GUI",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, hInstance, NULL
	);

	if (!hwnd_main) {
		MessageBox(NULL, L"CreateWindowEx failed!", L"Error", MB_ICONERROR);
		return;
	}

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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		
		for (uint32_t i = 0; i < game_state.resource_group_count; i++) {
			const resource_group_t* rg = &game_state.resource_groups[i];
			for (uint32_t j = 0; j < rg->resource_count; j++) {
				const resource_t* r = &rg->resources[j];
				wchar_t output[128];
				swprintf(output, _countof(output), L"%s", exp_to_str(r->amount));

				TextOutW(hdc, 20 + i * 20, 20 + j * 20, output, lstrlenW(output));
			}
		}
		wchar_t buffer[64] = { 0 };
		swprintf_s(buffer, _countof(buffer), L"%I64u", GetTickCount64()-start_time);
		SIZE t_size;
		GetTextExtentPoint32W(hdc, buffer, wcslen(buffer), &t_size);
		int p = 30;
		TextOutW(hdc, 0 + p, height - p, buffer, (int)wcslen(buffer));
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_TIMER:
		tick_calculate();
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_SIZE:
		get_window_size(hwnd);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_ACTIVATE:
		get_window_size(hwnd);
		return 0;
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		FreeConsole();
		PostQuitMessage(0);
		return 0;
	case WM_ERASEBKGND: {
		HDC hdc = (HDC)wParam;
		RECT rect;
		GetClientRect(hwnd, &rect);
		FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
		return 1;
	}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}