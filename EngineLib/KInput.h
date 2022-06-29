#pragma once
#include "KStd.h"

enum  KeyState
{
	KEY_FREE = 0,
	KEY_UP,
	KEY_PUSH,
	KEY_HOLD,
};
class KInput : public KSingleton<KInput>
{
	friend class KSingleton<KInput>;
private:
	DWORD	m_dwKeyState[256];
public:
	POINT	m_ptBeforePos;
	POINT   m_ptPos;
	POINT   m_pDragDown;
	POINT   m_pDrag;
	bool	m_bDrag;
	bool	m_bMove;
	int		m_iWheel = 0;

	DWORD   GetKey(DWORD dwKey);
	LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	void	OnMove(int iX, int iY);
	void	OnBegin(int iX, int iY);
	void	OnEnd();
private:
	KInput();
public:
	~KInput();
};

#define g_Input KInput::Get()