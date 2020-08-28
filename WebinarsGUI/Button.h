#pragma once
#include "framework.h"
#include <string>

class Button {
public:
	std::wstring name;
	HWND wnd;
	std::wstring cmd;
};