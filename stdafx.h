#pragma once

#include "targetver.h"

#include <afxdlgs.h>
#include <windows.h>

#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <DirectXMath.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11on12.h>
#include <wrl.h>
#include <vector>
#include <fbxsdk.h>
#include <stack>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

using namespace DirectX;
using namespace std;

#define FRAME_BUFFER_WIDTH 640
#define FRAME_BUFFER_HEIGHT 480
#define INVALID 4294967295

#define ASPECT_RATIO				(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))
