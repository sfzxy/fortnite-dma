#pragma once
#include "util.h"
#include "notification.h"

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;

namespace pic
{
    extern ID3D11ShaderResourceView* bg;
    extern ID3D11ShaderResourceView* widget;

    extern ID3D11ShaderResourceView* aimbot;
    extern ID3D11ShaderResourceView* visuals;
    extern ID3D11ShaderResourceView* skins;
    extern ID3D11ShaderResourceView* settings;

    extern ID3D11ShaderResourceView* items;
    extern ID3D11ShaderResourceView* radar;

    extern ID3D11ShaderResourceView* weapon_ak47;
    extern ID3D11ShaderResourceView* weapon_m4a1;
    extern ID3D11ShaderResourceView* weapon_galil;
    extern ID3D11ShaderResourceView* weapon_rifle;

    extern ID3D11ShaderResourceView* user_circle;
}

namespace fonts
{
	extern ImFont* inter_bold;
	extern ImFont* inter_default;
	extern ImFont* icon;
}

extern ImColor color_edit4;

class Render {
public:
	static HRESULT DirectXInit();
	static void CreateOverlay();
	static void CleanupD3D();
};