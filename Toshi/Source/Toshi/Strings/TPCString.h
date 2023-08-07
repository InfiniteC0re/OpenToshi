#pragma once
#include <Toshi/Strings/TString8.h>
#include <Toshi/Strings/TCStringPool.h>

namespace Toshi
{
#ifdef TOSHI_ENABLE_DEPRECATED
	class TPCString
	{
		TString8* m_sStr; //0x0
		mutable TString8 m_sStr2; //0x4
		TCStringPool* m_pStringPool; // 0xC

	public:
		inline TCStringPool* GetStringPool() const { return m_pStringPool; }
		inline TString8& GetVolatileCString() const { return m_sStr2; }
		inline const TString8& GetCString() const { return m_sStr == TNULL ? NullString : m_sStr2; }
		inline TBOOL IsEmpty() const { return m_sStr == TNULL ? TTRUE : m_sStr2.IsEmpty();; }

		TPCString() { m_sStr = TNULL; }
	};
#endif TOSHI_ENABLE_DEPRECATED
}


