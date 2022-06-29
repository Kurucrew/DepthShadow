#pragma once
#include "KStd.h"

class CStopwatch
{
public:
	CStopwatch() { QueryPerformanceFrequency(&m_liPerFreq); Start(); }
	void Start() { QueryPerformanceCounter(&m_liPerfStart); }

	__int64 Now()
	{    // Start �� ȣ��� �� ����� �и��ʸ� ����
		LARGE_INTEGER liPerfNow;
		QueryPerformanceCounter(&liPerfNow);
		__int64 qwElapsedTime = (((liPerfNow.QuadPart - m_liPerfStart.QuadPart)));
		Start();
		return qwElapsedTime;
	}
	void Output(const TCHAR* pData)
	{
#ifdef _DEBUG
		TCHAR  strMessage[MAX_PATH] = { 0, };
		__int64 qwElapsedTime = Now();
		float Second = static_cast<float>(qwElapsedTime) / static_cast<float>(m_liPerFreq.QuadPart);
		_stprintf_s(strMessage, _T("<=====%s[%lld:%10.4f]====>"), pData, qwElapsedTime, Second);
		OutputDebugString(strMessage);
#endif
	}
private:
	LARGE_INTEGER m_liPerFreq;    // �ʴ� ���
	LARGE_INTEGER m_liPerfStart;   // ���� ���
};
// 1,����ð� ����(�����÷��� �ð�)
// 2,1�������� ��� �ð�
// 3,1�ʿ� ����� ī����(FPS)
class KTimer
{
private:
	LARGE_INTEGER	m_Frequency;// ���ļ�
	LARGE_INTEGER	m_Current;
	LARGE_INTEGER	m_Frame;
	int				m_iTmpCounter;

public:
	int				m_iFPS;

	//�� ������ ������ �� ��� �ð� ��� (���ɰ� ������� ������ �ð��� �����ϵ���)
	float			m_fSecPerFrame;
	float			m_fGameTimer;
	TCHAR			m_szTimerString[MAX_PATH] = { 0, };
	TCHAR			m_szInfoString[MAX_PATH] = { 0, };

	bool			Init();
	bool			Frame();
	bool			Render();
	bool			Release();

	KTimer();
	virtual ~KTimer();
};

