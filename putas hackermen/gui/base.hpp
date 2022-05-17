#ifndef __GUI_H__
#define __GUI_H__

#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_internal.h"

#include <Windows.h>
#include <dwmapi.h>
#include <string>
#include <map>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")

namespace gui
{
	static LPDIRECT3D9				d3d9 = nullptr;
	static LPDIRECT3DDEVICE9		device = nullptr;
	static D3DPRESENT_PARAMETERS	d3dpp = { };

	static HWND						hWnd = nullptr;
	static POINTS					guiPosition = { };
	static WNDCLASSEX				wc = { };
	static const ImVec2				size = { 500, 500 };

	VOID CreateGuiApp(const char* windowName, const char* className);
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool InitGui();
	VOID Main();

	/// DIRECTX

	bool InitializeD3D();
	bool CreateDeviceD3D(HWND hWnd);
	VOID CleanupDevice();
	VOID ResetDevice();

	/// IMGUI

	VOID SetupImGui();
	VOID ShutdownImGui();
}

#endif