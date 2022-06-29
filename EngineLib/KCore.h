#pragma once
#include "KWindow.h"
#include "KDevice.h"
#include "KTimer.h"
#include "KInput.h"
#include "KWrite.h"
#include "KCamera.h"
#include "KDxState.h"

class KCore : public KWindow
{
public:
	ID3D11RasterizerState* m_pRSSolid;
	ID3D11RasterizerState* m_pRSWireFrame;

	KTimer			m_Timer;
	KWrite			m_Write;
	KDebugCamera	m_Camera;
	bool			m_bDebugText = false;

private:
	bool			GameInit()	override;
	bool			GameRun()	override;
	bool			GameFrame();
	bool			GameRender();
	bool			GameRelease()override;

public:
	void			FrameCamera();
	LRESULT			MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)override;

	virtual bool	Init();
	virtual bool	Frame();
	virtual bool	PreRender();
	virtual bool	Render();
	virtual bool	PostRender();
	virtual bool	Release();
};

