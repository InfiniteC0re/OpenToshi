#include "ToshiPCH.h"
#include "TInputInterface.h"

TOSHI_NAMESPACE_START

TINT TInputDevice::ProcessRepeats(EventEmitter& a_rEmitter, TFLOAT a_fDeltaTime)
{
	TINT iNumProcessed = 0;

	for (TINT i = 0; i < m_Repeats.GetNumElements(); i++)
	{
		auto& rRepeat = m_Repeats[i];

		if (IsDown(rRepeat.iDoodad))
		{
			rRepeat.fLeftTime -= a_fDeltaTime;

			if (rRepeat.fLeftTime <= 0.0f)
			{
				rRepeat.fLeftTime = rRepeat.fRepeatTime;
				ThrowRepeatEvent(a_rEmitter, &rRepeat, a_fDeltaTime);
				iNumProcessed += 1;
			}
		}
		else
		{
			if (1 < m_Repeats.GetNumElements())
			{
				auto& rLastRepeat   = m_Repeats[m_Repeats.GetNumElements() - 1];
				rRepeat.iDoodad     = rLastRepeat.iDoodad;
				rRepeat.fLeftTime   = rLastRepeat.fLeftTime;
				rRepeat.fRepeatTime = rLastRepeat.fRepeatTime;
			}

			if (0 < m_Repeats.GetNumElements())
			{
				m_Repeats.Pop();
			}
		}
	}

	return iNumProcessed;
}

TBOOL TInputDevice::StartRepeat(Doodad a_iDoodad, TFLOAT a_fFirstRepeatTime, TFLOAT a_fRepeatTime)
{
	TArray<RepeatInfo> it;
	m_Repeats.Bind(it);

	while (!it.IsOver())
	{
		if (it->iDoodad == a_iDoodad)
		{
			it->fRepeatTime = a_fRepeatTime;
			return TTRUE;
		}

		it++;
	}

	m_Repeats.Push({ .iDoodad = a_iDoodad, .fLeftTime = a_fFirstRepeatTime, .fRepeatTime = a_fRepeatTime });

	return TTRUE;
}

void TInputDevice::StopRepeat(Doodad a_iDoodad)
{
	if (m_Repeats.GetNumElements() != 0)
	{
		TArray<RepeatInfo> it;
		m_Repeats.Bind(it);

		TINT iIndex = 0;
		while (it->iDoodad != a_iDoodad)
		{
			it++;
			iIndex++;
			if (it.IsOver()) return;
		}

		if (1 < m_Repeats.GetNumElements())
		{
			auto& rLastRepeat = m_Repeats[m_Repeats.GetNumElements() - 1];
			it->iDoodad       = rLastRepeat.iDoodad;
			it->fLeftTime     = rLastRepeat.fLeftTime;
			it->fRepeatTime   = rLastRepeat.fRepeatTime;
		}

		if (0 < m_Repeats.GetNumElements())
		{
			m_Repeats.Pop();
		}
	}
}

void TInputDevice::StopAllRepeats()
{
	m_Repeats.Clear();
}

void TInputDevice::ThrowRepeatEvent(EventEmitter& emitter, RepeatInfo* repeatInfo, TFLOAT deltaTime)
{
	emitter.Throw(TInputInterface::InputEvent(this, repeatInfo->iDoodad, TInputInterface::EVENT_TYPE_REPEAT));
}

TOSHI_NAMESPACE_END
