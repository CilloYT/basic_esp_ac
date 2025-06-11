#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <dwmapi.h>
#include <d3d11.h>

#include <../imgui/imgui.h>
#include <../imgui/imgui_impl_dx11.h>
#include <../imgui/imgui_impl_win32.h>

#include "memory/memory.h"
#include "offsets/offsets.h"
#include "calc/math.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
	{
		return 0L;
	}


	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProc(window, message, w_param, l_param);
}



INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show)
{

	//Window Setup

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = instance;
	wc.lpszClassName = L"ExternelOverlayClass";

	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		wc.lpszClassName,
		L"External ESP for AC (Cillo)",
		WS_POPUP,
		0,
		0,
		1920,
		1080,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);


	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);


	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins
		{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	constexpr D3D_FEATURE_LEVEL levels[2]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer)
	{
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else
	{
		return 1;
	}

	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);

	bool running = true;


	//declare memory instance
	memory mem;

	//get processID
	HWND hwnd = FindWindowA(0, "AssaultCube");
	DWORD pID;
	GetWindowThreadProcessId(hwnd, &pID);

	//get baseAddress
	DWORD baseAddr;
	baseAddr = mem.GetModuleBaseAddress(pID, L"ac_client.exe");

	std::cout << baseAddr << "\n\n";

	//get HANDLE with ALL_ACCESS
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, pID);

	uintptr_t locPlayer = mem.readMemory(baseAddr + offsets::localPlayer, handle);
	uintptr_t locHealth = mem.readMemory(locPlayer + offsets::health, handle);
	std::cout << "health: " << locHealth << "\n\n";

	uintptr_t entityList = mem.readMemory(baseAddr + offsets::entityList, handle);
	uintptr_t currentEntity;
	char name[32];
	int entHealth;

	float viewmatrix[16];
	Vector2 screen;
	Vector3 pos;
	Math math;

	while (running)
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				running = false;
			}
		}


		if (!running)
		{
			break;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		// RENDERING
		

		ReadProcessMemory(handle, (LPCVOID)(offsets::viewMatrix), &viewmatrix, sizeof(viewmatrix), nullptr); //viewmatrix set
		//loop entList
		for (int i = 1; i < 8; i++)
		{
			currentEntity = mem.readMemory(entityList + 0x4 * i, handle);
			entHealth = mem.readMemory(currentEntity + offsets::health, handle);
			ReadProcessMemory(handle, (LPCVOID)(currentEntity + offsets::name), &name, sizeof(name), nullptr); //entitys name set



			ReadProcessMemory(handle, (LPCVOID)(currentEntity + offsets::entityPosX), &pos.x, sizeof(pos.x), nullptr);
			ReadProcessMemory(handle, (LPCVOID)(currentEntity + offsets::entityPosY), &pos.y, sizeof(pos.y), nullptr);
			ReadProcessMemory(handle, (LPCVOID)(currentEntity + offsets::entityPosZ), &pos.z, sizeof(pos.z), nullptr);


			if (math.WorldToScreen(pos, screen, viewmatrix, 1920, 1061))
			{
				std::cout << name << " - x: " << screen.x << " - y: " << screen.y << "\n";
				ImGui::GetBackgroundDrawList()->AddCircleFilled({ screen.x, screen.y }, 10.f, ImColor(1.f, 0.f, 0.f));
			}


		}


		// RENDERING LAST
		ImGui::Render();

		constexpr float color[4]{ 0.f, 0.f, 0.f, 0.f };
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(1U, 0U);



	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	if (swap_chain)
		swap_chain->Release();

	if (device_context)
		device_context->Release();

	if (device)
		device->Release();

	if (render_target_view)
		render_target_view->Release();


	DestroyWindow(window);

	UnregisterClassW(wc.lpszClassName, wc.hInstance);



	return 0;

}