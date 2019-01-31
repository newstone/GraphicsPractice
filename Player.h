#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "Object.h"
#include "Camera.h"

class Player : public Object
{
private:
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	XMFLOAT3					m_xmf3Velocity; 
	XMFLOAT3     				m_xmf3Gravity;
	float           			m_fMaxVelocityXZ;
	float           			m_fMaxVelocityY;
	float           			m_fFriction;

	LPVOID						m_pPlayerUpdatedContext;
	LPVOID						m_pCameraUpdatedContext;

	Camera						*m_pCamera = NULL;
public:
	Player();
	~Player();
	void SetCamera(Camera* pCamera)
	{
		m_pCamera = pCamera;
	}

	const XMFLOAT3& GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	const XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3 GetPosition() { return m_xmf3Position; }

	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }

	void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity);
	void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Rotate(float x, float y, float z);
	void Update(float fTimeElapsed);
};

