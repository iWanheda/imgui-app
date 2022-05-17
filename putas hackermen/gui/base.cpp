#include "base.hpp"

///
/// GUI
/// 

// forward declaration for wnd proc handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

VOID gui::CreateGuiApp(const char* windowName, const char* className)
{
	gui::wc = { sizeof(WNDCLASSEX), CS_CLASSDC, gui::WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, className, NULL };
	::RegisterClassEx(&wc);
	hWnd = ::CreateWindow(wc.lpszClassName, windowName, WS_POPUP, 100, 100, gui::size.x, gui::size.y, NULL, NULL, wc.hInstance, NULL);
	
	MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(hWnd, &margins);

	InitializeD3D();

	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);
}

LRESULT WINAPI gui::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	
	switch (msg)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xFFF0) == SC_KEYMENU) // Disable ALT application menu
			return 0L;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0L;
	case WM_LBUTTONDOWN:
		guiPosition = MAKEPOINTS(lParam);
		return 0L;
	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(lParam);
			auto rect = ::RECT { };
			
			GetWindowRect(hWnd, &rect);
			
			rect.left += points.x - guiPosition.x;
			rect.top += points.y - guiPosition.y;
			
			if (guiPosition.x >= 0 &&
				guiPosition.x <= rect.right &&
				guiPosition.y >= 0 && guiPosition.y <= 19)
			{
				SetWindowPos(
					hWnd,
					HWND_TOPMOST,
					rect.left, rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
			}
		}
		
		return 0L;
	}
	
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool gui::InitGui()
{
	return TRUE;
}

// TODO IMPORTANT: COPY THIS & USE DEFAULT IMGUI TO GET DECENT YES

VOID gui::Main()
{
	SetupImGui();
	InitGui();

	bool loaderOpen = true;
	while (loaderOpen)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				loaderOpen = false;
		}

		if (!loaderOpen)
			break;

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize(gui::size);
		ImGui::Begin("ImGui Loader", nullptr, ImGuiWindowFlags_NoResize);
		{

		}
		ImGui::End();

		ImGui::EndFrame();

		device->SetRenderState(D3DRS_ZENABLE, FALSE);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

		device->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, NULL, 1.f, NULL);
		if (device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			device->EndScene();
		}

		HRESULT result = device->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	ShutdownImGui();
}

///
///	DIRECT X
///

bool gui::CreateDeviceD3D(HWND hWnd)
{
	if ((gui::d3d9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return FALSE;

	// Create the D3D Device
	ZeroMemory(&gui::d3dpp, sizeof(gui::d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // vSync enabled

	if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &device) < 0)
		return false;

	return true;
}

bool gui::InitializeD3D()
{
	if (!CreateDeviceD3D(hWnd))
	{
		// Cleanup
		CleanupDevice();

		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return TRUE;
	}
}

VOID gui::CleanupDevice()
{
	if (gui::device) { device->Release(); device = nullptr; }
	if (gui::d3d9) { d3d9->Release(); d3d9 = nullptr; }
}

VOID gui::ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT hr = device->Reset(&d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(NULL);

	ImGui_ImplDX9_CreateDeviceObjects();
}

///
/// IMGUI
/// 

VOID gui::SetupImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// not sure if needed
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(gui::device);
}

VOID gui::ShutdownImGui()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDevice();
	::DestroyWindow(hWnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
}