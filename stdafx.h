#pragma once

#include "targetver.h"

#include <windows.h>

#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <DirectXMath.h>
#include <d2d1_3.h>
#include <d3d11on12.h>
#include <wrl.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

using namespace DirectX;

#define FRAME_BUFFER_WIDTH 100
#define FRAME_BUFFER_HEIGHT 100

