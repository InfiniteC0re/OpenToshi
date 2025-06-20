#pragma once

TOSHI_NAMESPACE_START

class THPTimer
{
public:
	THPTimer() { Reset(); }

	TUINT32 GetRaw32()
	{
		LARGE_INTEGER raw32;
		QueryPerformanceCounter(&raw32);
		return static_cast<TUINT32>(raw32.QuadPart);
	}

	int64_t GetRaw64()
	{
		LARGE_INTEGER raw64;
		QueryPerformanceCounter(&raw64);
		return raw64.QuadPart;
	}

	void Reset()
	{
		QueryPerformanceCounter(&m_iCurrentTime);
		QueryPerformanceFrequency(&m_iFrequency);
		m_iOldTime = m_iCurrentTime;
		m_fDelta   = 0;
	}

	void Update()
	{
		m_iOldTime = m_iCurrentTime;
		QueryPerformanceCounter(&m_iCurrentTime);

		TFLOAT ratio      = 1.0f / m_iFrequency.QuadPart;
		m_fDelta          = (m_iCurrentTime.QuadPart - m_iOldTime.QuadPart) * ratio;
		m_fCurrentSeconds = m_iCurrentTime.QuadPart * ratio;
	}

public:
	inline TFLOAT GetDelta() const { return m_fDelta; }
	inline TFLOAT GetCurrentSeconds() const { return m_fCurrentSeconds; }

private:
	LARGE_INTEGER m_iFrequency;      // 0x0
	TFLOAT        m_fCurrentSeconds; // 0x8
	LARGE_INTEGER m_iOldTime;        // 0x10
	LARGE_INTEGER m_iCurrentTime;    // 0x18
	TFLOAT        m_fDelta;          // 0x20
};

TOSHI_NAMESPACE_END
