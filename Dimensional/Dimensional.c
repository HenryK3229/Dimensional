#include <windows.h>
#include <stdio.h>

#include "structure_data.h"
#include "gui.h"
#include "game.h"



int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);

    game_init(NULL);

    const wchar_t CLASS_NAME[] = L"SimpleWindowClass";

    gui_init(hInstance, nCmdShow);
    gui_run();
    return 0;
}