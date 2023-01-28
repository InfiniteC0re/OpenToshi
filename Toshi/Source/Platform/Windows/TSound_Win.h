#pragma once
#include "Toshi/Sound/TSound.h"

namespace Toshi
{
	class TSound_Win : public TSound
	{
	public:
		bool Initialise(int maxchannels, int unk);

		static FMOD_SYSTEM_CALLBACK SoundCB;
		
	};
	
}


