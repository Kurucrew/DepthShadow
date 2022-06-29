#pragma once
#include "KStd.h"
class KCamera
{
public:
	float				m_pSpeed;
	KVector3			m_vCameraPos;
	KVector3			m_vCameraTarget;
	KVector3			m_vLook;
	KVector3			m_vSide;
	KVector3			m_vUp;
	float				m_fYaw = 0.0f;
	float				m_fPitch = 0.0f;
	float				m_fRoll = 0.0f;
	float				m_fRadius = 10.0f;

	KMatrix				m_matWorld;
	KMatrix				m_matView;
	KMatrix				m_matProj;

	virtual KMatrix     CreateViewMatrix(KVector3 vPos, KVector3 vTarget, KVector3 vUp = KVector3(0, 1, 0));
	virtual KMatrix  	CreateProjMatrix(float fNear, float fFar, float fFov, float fAspect);

	virtual bool		Init();
	virtual bool		Frame();
	virtual bool		Render();
	virtual bool		Release();

	KCamera();
	virtual ~KCamera();
};

class KDebugCamera : public KCamera
{
public:
	virtual bool			Frame() override;
	virtual KMatrix			Update(KVector4 vValue);
};

