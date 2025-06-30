#pragma once

#include <windows.h>
#include "structure_data.h"


void gui_init( HINSTANCE hInstance, int nCmdShow);
void gui_run(void);
void format_resource_amount(wchar_t* dest, size_t size, const resource_t* r);