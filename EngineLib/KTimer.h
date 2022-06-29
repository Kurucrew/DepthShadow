#pragma once
#include "KStd.h"

class CStopwatch
{
public:
	CStopwatch() { QueryPerformanceFrequency(&m_liPerFreq); Start(); }
	void Start() { QueryPerformanceCounter(&m_liPerfStart); }

	__int64 Now()
	{    // Start 가 호출된 후 경과한 밀리초를 리턴
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
	LARGE_INTEGER m_liPerFreq;    // 초당 계수
	LARGE_INTEGER m_liPerfStart;   // 시작 계수
};
// 1,실행시간 누적(게임플레이 시간)
// 2,1프레임의 경과 시간
// 3,1초에 실행된 카운터(FPS)
class KTimer
{
private:
	LARGE_INTEGER	m_Frequency;// 주파수
	LARGE_INTEGER	m_Current;
	LARGE_INTEGER	m_Frame;
	int				m_iTmpCounter;

public:
	int				m_iFPS;

	//이 값으로 프레임 별 출력 시간 계산 (성능과 관계없이 동일한 시간대 진행하도록)
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

