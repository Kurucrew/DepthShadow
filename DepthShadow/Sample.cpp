#include "Sample.h"

void Sample::TexSet()
{
	m_matTex._11 = 0.5f;
	m_matTex._22 = -0.5f;
	m_matTex._41 = 0.5f;
	m_matTex._42 = 0.5f;

	m_DiffuseLight.Set(KVector3(150, 180, 150), KVector3(0, 0, 0));
}
void Sample::MapCreate()
{
	m_MiniMap.Create(L"Projection.hlsl", L"Projection.hlsl", L"");

	m_MapObj.Create(L"Projection.hlsl", L"Projection.hlsl", L"../../data/BK3.jpg");

	KMatrix matWorld, matScale;
	D3DXMatrixRotationX(&matWorld, XM_PI / 2.0f);
	D3DXMatrixScaling(&matScale, 200.0f, 200.0f, 200.0f);
	m_MapObj.m_matWorld = matScale * matWorld;
	m_Rt.Create(1024, 1024);
}
bool Sample::LoadShader()
{
	ID3DBlob* PSBlob = nullptr;
	PSBlob = KModel::LoadShaderBlob(L"CharacterShader.hlsl", "PSShadow", "ps_5_0");
	if (PSBlob != nullptr)
	{
		HRESULT hr = S_OK;
		hr = g_pd3dDevice->CreatePixelShader(
			PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &m_pPSShadow);
		if (FAILED(hr)) { return hr; }
		PSBlob->Release();
	}
	m_FbxCharacter.LoadObject("../../data/object/man.fbx", "CharacterShader.hlsl");
	return true;
}
void Sample::InitCamera()
{
	m_Camera.CreateViewMatrix(KVector3(0, 0, -100), KVector3(0, 0, 0));
	m_Camera.CreateProjMatrix(1.0f, 1000.0f, XM_PI * 0.25f, (float)g_rtClient.right / (float)g_rtClient.bottom);
}
bool Sample::Init()
{
	TexSet();
	MapCreate();
	LoadShader();
	InitCamera();

	return true;
}
bool Sample::Frame()
{
	if (g_Input.GetKey(VK_F4) == KEY_PUSH)
	{
		m_FbxCharacter.m_bAnimPlay = !m_FbxCharacter.m_bAnimPlay;
	}
	m_FbxCharacter.Frame();
	m_DiffuseLight.Frame();
	m_ShadowCB.g_matShadow = m_DiffuseLight.m_matView * m_DiffuseLight.m_matProj * m_matTex;
	return true;
}

void Sample::SetTextBox()
{
	//Text 영역 설정 및 컬러 설정
	RECT  rt = { 400, 0, 800, 600 };
	m_Write.DrawText(rt, m_Timer.m_szTimerString, D2D1::ColorF(1, 1, 1, 1));
	RECT  rt2 = { 700, 550, 0, 200 };
	m_Write.DrawText(rt2, m_Timer.m_szInfoString, D2D1::ColorF(1, 1, 1, 1));
}

bool Sample::Render()
{
	ApplyRS(m_pImmediateContext, KDxState::g_pRSSolid);

	if (m_Rt.Begin(m_pImmediateContext))
	{
		//m_Camera -> m_DiffuseLight 로 카메라 위치 변경
		m_FbxCharacter.SetMatrix(&m_FbxCharacter.m_matWorld, &m_DiffuseLight.m_matView, &m_DiffuseLight.m_matProj);
		m_FbxCharacter.SetPixelShader(m_pPSShadow);
		m_FbxCharacter.Render(m_pImmediateContext);
		m_Rt.End(m_pImmediateContext);
	}
	ApplySS(m_pImmediateContext, KDxState::g_pClampSS, 1);
	m_MapObj.m_cbData.matNormal = m_ShadowCB.g_matShadow;
	m_MapObj.SetMatrix(&m_MapObj.m_matWorld, &m_Camera.m_matView, &m_Camera.m_matProj);
	m_pImmediateContext->PSSetShaderResources(1, 1, &m_Rt.m_pTextureSRV);
	m_MapObj.Render(m_pImmediateContext);

	m_MiniMap.SetMatrix(nullptr, nullptr, nullptr);
	m_MiniMap.PreRender(m_pImmediateContext);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_Rt.m_pTextureSRV);
	m_MiniMap.PostRender(m_pImmediateContext, m_MiniMap.m_iNumIndex);

	m_FbxCharacter.SetMatrix(&m_FbxCharacter.m_matWorld, &m_Camera.m_matView, &m_Camera.m_matProj);
	m_FbxCharacter.SetPixelShader(nullptr);
	m_FbxCharacter.Render(m_pImmediateContext);

	SetTextBox();

	return true;
}



bool Sample::Release()
{
	m_MapObj.Release();
	m_MiniMap.Release();
	m_Rt.Release();
	m_FbxCharacter.Release();
	SAFE_RELEASE(m_pPSShadow);
	return true;
}

Sample::Sample() {}
Sample::~Sample() {}

KGAME_RUN(DepthShadow)



