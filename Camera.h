#pragma once

class Player;

struct CameraInfo
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT3						    m_xmf3CameraPosition;
};

class Camera
{
public:
	Camera();
	virtual ~Camera();

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	XMFLOAT4X4					m_xmf4x4View;
	XMFLOAT4X4					m_xmf4x4Projection;
	XMFLOAT4X4					m_xmf4x4ViewProject;

	D3D12_VIEWPORT					m_Viewport;
	D3D12_RECT						m_d3dScissorRect;

	ID3D12Resource					*m_pd3dcbCamera = nullptr;
	CameraInfo				*m_pcbMappedCamera = nullptr;
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle);
	void SetViewport(int xStart, int yStart, int nWidth, int nHeight);
	void SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	void SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
	void SetLookAt(XMFLOAT3& vPosition, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void Move(const XMFLOAT3& xmf3Shift);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	const D3D12_VIEWPORT* GetViewport() { return(&m_Viewport); }
	const D3D12_RECT* GetScissorRect() { return(&m_d3dScissorRect); }

	void Update(Player *pPlayer, XMFLOAT3& xmf3LookAt, float fTimeElapsed = 0.016f);
};