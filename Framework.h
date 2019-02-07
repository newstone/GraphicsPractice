#pragma once
#include "Scene.h"
#include "Camera.h"

class Framework
{
private:
	HWND m_hWnd;

	UINT m_nWndClientWidth;
	UINT m_nWndClientHeight;

	ComPtr<IDXGIFactory4>            m_dxgiFactory;
	ComPtr<IDXGISwapChain3>            m_dxgiSwapChain ;
	ComPtr<ID3D12Device>            m_d3dDevice;

	UINT m_dxgiFactoryFlags;
	static const UINT            m_nSwapChainBuffers = 2;

	bool                  m_bMsaa4xEnable = false;
	UINT                  m_nMsaa4xQualityLevels = 0;

	ComPtr<ID3D12Fence>               m_d3dFence;
	UINT64                  m_nFenceValues[m_nSwapChainBuffers];
	HANDLE                  m_hFenceEvent;

	ComPtr<ID3D12CommandQueue> m_d3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_d3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_d3dCommandList;

	ComPtr<ID3D12DescriptorHeap>      m_d3dRtvDescriptorHeap;
	UINT                  m_nRtvDescriptorIncrementSize;
	ComPtr<ID3D12Resource>            m_d3dSwapChainBackBuffers[m_nSwapChainBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE      m_pd3dRtvSwapChainBackBufferCPUHandles[m_nSwapChainBuffers];

	ComPtr<ID3D12Resource>            m_d3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap>      m_d3dDsvDescriptorHeap;
	UINT                  m_nDsvDescriptorIncrementSize;
	UINT				  m_nSwapChainBufferIndex;

	Scene* m_pScene;
	Camera* m_pCamera;
	Player* m_pPlayer;

	POINT						m_ptOldCursorPos;
public:
	Framework();
	~Framework();

	void OnCreate(HWND hMainWnd);
	void CreateDevice();
	void CreateQueue();
	void CreateDescriptHeap(); 
	void CreateSwapChainRenderTargetViews();
	void CreateSwapChain();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void LoadFBXFile(const string& strPath);
	void BuildScene();

	void OnResizeBackBuffers();
	void WaitForGpuComplete(); 
	void MoveToNextFrame();

	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void ProcessInput();
	void Update();
	void Run();
};

