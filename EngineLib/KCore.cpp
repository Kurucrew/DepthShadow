#include "KCore.h"

LRESULT KCore::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return g_Input.MsgProc(hWnd, message, wParam, lParam);
}
void KCore::FrameCamera()
{
    if (g_Input.m_bDrag && g_Input.m_ptBeforePos.x == g_Input.m_pDragDown.x)
    {
        g_Input.m_pDrag.x = 0;
    }
    if (g_Input.m_bDrag && g_Input.m_ptBeforePos.y == g_Input.m_pDragDown.y)
    {
        g_Input.m_pDrag.y = 0;
    }
    float fYaw = g_fSecPerFrame * g_Input.m_pDrag.x * 5.0f;
    float fPitch = g_fSecPerFrame * g_Input.m_pDrag.y * 5.0f;
    m_Camera.Update(KVector4(fPitch, fYaw, 0.0f, 0.0f));
    m_Camera.Frame();
}
bool KCore::GameRun()
{
    if (!GameFrame()) { return false; }
    if (!GameRender()) { return false; }
    return true;
}
bool KCore::GameInit()
{
    KDevice::SetDevice();
    KDxState::Init();

    m_Timer.Init();
    g_Input.Init();
    m_Write.Init();

    //카메라 초기 좌표값 세팅
    m_Camera.Init();
    m_Camera.CreateViewMatrix(KVector3(0, 0, -100), KVector3(0, 0, 0));
    m_Camera.CreateProjMatrix(1.0f, 1000.0f, XM_PI * 0.25f, (float)g_rtClient.right / (float)g_rtClient.bottom);

    IDXGISurface1* m_pBackBuffer;
    m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&m_pBackBuffer);
    m_Write.CreateDeviceResources(m_pBackBuffer);
    if (m_pBackBuffer)m_pBackBuffer->Release();

    Init();
    return true;
}
bool KCore::GameFrame()
{
    m_Timer.Frame();
    g_Input.Frame();
    m_Write.Frame();
    FrameCamera();

    if (g_Input.GetKey(VK_F2) >= KEY_PUSH)
    {
        ApplyRS(m_pImmediateContext, KDxState::g_pRSWireFrame);
    }
    else
    {
        ApplyRS(m_pImmediateContext, KDxState::g_pRSSolid);
    }

    Frame();
    g_Input.m_ptBeforePos = g_Input.m_ptPos;
    return true;
}
bool KCore::GameRender()
{
    PreRender();
    m_Timer.Render();
    g_Input.Render();
    m_Write.Render();

    if (KDxState::g_pRSWireFrame)
    {
        m_pImmediateContext->RSSetState(KDxState::g_pRSWireFrame);
    }
    else
        m_pImmediateContext->RSSetState(KDxState::g_pCurrentRS);
    Render();
    PostRender();
    return true;
}
bool KCore::GameRelease()
{
    Release();
    KDxState::Release();
    m_Timer.Release();
    g_Input.Release();
    m_Write.Release();
    m_Camera.Release();
    CleanupDevice();
    return true;
}
bool KCore::Init()
{
    return true;
}
bool KCore::Frame()
{
    return true;
}
bool KCore::PreRender()
{
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
    m_pImmediateContext->ClearRenderTargetView(
        m_DefaultRT.m_pRenderTargetView, ClearColor);
    m_pImmediateContext->ClearDepthStencilView(
        m_DefaultDS.m_pDepthStencilView,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pImmediateContext->OMSetRenderTargets(
        1, &m_DefaultRT.m_pRenderTargetView, m_DefaultDS.m_pDepthStencilView);

    ApplyDSS(m_pImmediateContext, KDxState::g_pLessEqualDSS);
    ApplySS(m_pImmediateContext, KDxState::g_pWrapSS, 0);
    ApplyRS(m_pImmediateContext, KDxState::g_pRSWireFrame);

    return true;
}
bool KCore::Render()
{
    return true;
}
bool KCore::PostRender()
{
    assert(m_pSwapChain);
    m_pSwapChain->Present(0, 0);
    return true;
}
bool KCore::Release()
{
    return true;
}