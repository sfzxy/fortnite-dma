#include "render.h"

#include "font1.h"
#include "image.h"

ImColor color_edit4 = { 110 / 255.f, 117 / 255.f, 255 / 255.f, 255.00f };

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

namespace pic
{
	ID3D11ShaderResourceView* bg = nullptr;
	ID3D11ShaderResourceView* widget = nullptr;

	ID3D11ShaderResourceView* aimbot = nullptr;
	ID3D11ShaderResourceView* visuals = nullptr;
	ID3D11ShaderResourceView* items = nullptr;
	ID3D11ShaderResourceView* radar = nullptr;
	ID3D11ShaderResourceView* skins = nullptr;
	ID3D11ShaderResourceView* settings = nullptr;

	ID3D11ShaderResourceView* weapon_ak47 = nullptr;
	ID3D11ShaderResourceView* weapon_m4a1 = nullptr;
	ID3D11ShaderResourceView* weapon_galil = nullptr;
	ID3D11ShaderResourceView* weapon_rifle = nullptr;


	ID3D11ShaderResourceView* user_circle = nullptr;
}

namespace fonts
{
	ImFont* inter_bold = nullptr;
	ImFont* inter_default = nullptr;
	ImFont* icon = nullptr;
}

HWND GameHwnd = NULL;
RECT GameRect = { NULL };
HWND MyHwnd = NULL;
HWND hijacked_hwnd = NULL;
MSG Message = { NULL };

uintptr_t procid = NULL;

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

HRESULT Render::DirectXInit()
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = MyHwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImFontConfig cfg;
	cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;

	fonts::inter_default = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	fonts::inter_bold = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	fonts::icon = io.Fonts->AddFontFromMemoryTTF(icon_font, sizeof(icon_font), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(MyHwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
	if (pic::bg == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, background, sizeof(background), &info, pump, &pic::bg, 0);
	if (pic::widget == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, widget_icon, sizeof(widget_icon), &info, pump, &pic::widget, 0);

	if (pic::aimbot == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, aimbot, sizeof(aimbot), &info, pump, &pic::aimbot, 0);
	if (pic::visuals == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, visuals, sizeof(visuals), &info, pump, &pic::visuals, 0);

	if (pic::items == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, items, sizeof(items), &info, pump, &pic::items, 0);
	if (pic::radar == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, radar, sizeof(radar), &info, pump, &pic::radar, 0);

	if (pic::skins == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, skins, sizeof(skins), &info, pump, &pic::skins, 0);
	if (pic::settings == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, settings, sizeof(settings), &info, pump, &pic::settings, 0);

	if (pic::weapon_ak47 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, weapon_ak47, sizeof(weapon_ak47), &info, pump, &pic::weapon_ak47, 0);
	if (pic::weapon_m4a1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, weapon_m4a1, sizeof(weapon_m4a1), &info, pump, &pic::weapon_m4a1, 0);
	if (pic::weapon_galil == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, weapon_galil, sizeof(weapon_galil), &info, pump, &pic::weapon_galil, 0);
	if (pic::weapon_rifle == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, weapon_rifle, sizeof(weapon_rifle), &info, pump, &pic::weapon_rifle, 0);


	if (pic::user_circle == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, user_circle, sizeof(user_circle), &info, pump, &pic::user_circle, 0);

	return S_OK;
}

void Render::CreateOverlay()
{
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		("Asgard"),
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	RECT Rect;
	GetWindowRect(GetDesktopWindow(), &Rect);

	RegisterClassExA(&wcex);

	MyHwnd = CreateWindowExA(NULL, ("Asgard"), ("Asgard"), WS_POPUP, 0, 0, Width, Height, NULL, NULL, NULL, NULL);
	SetWindowLong(MyHwnd, GWL_EXSTYLE,  WS_EX_TRANSPARENT ); // WS_EX_LAYERED |  | WS_EX_TOOLWINDOW
	SetLayeredWindowAttributes(MyHwnd, RGB(0, 0, 0), 255, LWA_ALPHA); // LWA_COLORKEY bad

	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(MyHwnd, &margin);

	ShowWindow(MyHwnd, SW_SHOW);
	UpdateWindow(MyHwnd);
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void Render::CleanupD3D()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(MyHwnd);
	//::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}