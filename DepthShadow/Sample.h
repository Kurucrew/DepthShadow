#pragma once
#include "KCore.h"
#include "KFbxObj.h"
#include "KShape.h"
#include "KMap.h"
class KMiniMap : public KPlaneShape
{
public:
	bool LoadTexture()
	{
		m_Tex.LoadTexture(L"../../data/baseColor.jpg");
	}
	bool CreateVertexData()
	{
		m_pVertexList.resize(4);

		m_pVertexList[0].pos = KVector3(-1.0f, 1.0f, 0.0f);
		m_pVertexList[0].normal = KVector3(0.0f, 0.0f, -1.0f);
		m_pVertexList[0].color = KVector4(1.0f, 1.0f, 1.0f, 1.0f);
		m_pVertexList[0].tex = KVector2(0.0f, 0.0f);

		m_pVertexList[1].pos = KVector3(-0.5f, 1.0f, 0.0f);
		m_pVertexList[1].normal = KVector3(0.0f, 0.0f, -1.0f);
		m_pVertexList[1].color = KVector4(1.0f, 1.0f, 1.0f, 1.0f);
		m_pVertexList[1].tex = KVector2(1.0f, 0.0f);

		m_pVertexList[2].pos = KVector3(-1.0f, 0.5f, 0.0f);
		m_pVertexList[2].normal = KVector3(0.0f, 0.0f, -1.0f);
		m_pVertexList[2].color = KVector4(1.0f, 1.0f, 1.0f, 1.0f);
		m_pVertexList[2].tex = KVector2(0.0f, 1.0f);

		m_pVertexList[3].pos = KVector3(-0.5f, 0.5f, 0.0f);
		m_pVertexList[3].normal = KVector3(0.0f, 0.0f, -1.0f);
		m_pVertexList[3].color = KVector4(1.0f, 1.0f, 1.0f, 1.0f);
		m_pVertexList[3].tex = KVector2(1.0f, 1.0f);

		return true;
	}
};
struct cbDataShadow
{
	KMatrix g_matShadow;
};
class KLight
{
public:
	KVector3	m_vPos;
	KVector3	m_vInitPos;
	KVector3	m_vTarget;
	KVector3	m_vDir;
	KMatrix	m_matWorld;
	KMatrix	m_matView;
	KMatrix	m_matProj;
public:
	void Set(KVector3 vPos, KVector3 vTarget)
	{
		m_vInitPos = vPos;
		m_vPos = vPos;
		m_vTarget = vTarget;

		m_vDir = m_vTarget - m_vPos;
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		KVector3 vUp(0, 1, 0);
		D3DXMatrixLookAtLH(&m_matView, &m_vPos, &m_vTarget, &vUp);
		D3DXMatrixPerspectiveFovLH(&m_matProj, XM_PI * 0.25f, 1.0f, 1.0f, 500.0f);
	}
	bool Frame()
	{
		D3DXMatrixRotationY(&m_matWorld, XM_PI * g_fGameTimer * 0.1f);
		D3DXVec3TransformCoord(&m_vPos, &m_vInitPos, &m_matWorld);

		m_vDir = m_vTarget - m_vPos;
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		KVector3 vUp(0, 1, 0);
		D3DXMatrixLookAtLH(&m_matView, &m_vPos, &m_vTarget, &vUp);
		D3DXMatrixPerspectiveFovLH(&m_matProj, XM_PI * 0.25f, 1.0f, 1.0f, 5000.0f);

		return true;
	}
};
class Sample :public KCore
{
public:
	KMatrix			   m_matTex;
	cbDataShadow	   m_ShadowCB;
	KLight			   m_DiffuseLight;
	KMiniMap		   m_MiniMap;
	KPlaneShape		   m_MapObj;
	KViewRT			   m_Rt;
	KFbxObj			   m_FbxCharacter;
	KMatrix			   m_matShadow;
	ID3D11PixelShader* m_pPSShadow = nullptr;
public:
	bool Init()override;
	void TexSet();
	void MapCreate();
	bool LoadShader();
	void InitCamera();

	bool Frame()override;
	bool Render()override;
	void SetTextBox();

	bool Release()override;
public:
	Sample();
	virtual ~Sample();
};

