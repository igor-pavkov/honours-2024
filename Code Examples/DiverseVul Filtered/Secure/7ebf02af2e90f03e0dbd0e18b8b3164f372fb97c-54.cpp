	void Reset()
	{
		plugParams.clear();
		elapsedTime = 0.0;
		state->m_lTotalSampleCount = 0;
		state->m_nMusicSpeed = sndFile.m_nDefaultSpeed;
		state->m_nMusicTempo = sndFile.m_nDefaultTempo;
		state->m_nGlobalVolume = sndFile.m_nDefaultGlobalVolume;
		chnSettings.assign(sndFile.GetNumChannels(), ChnSettings());
		for(CHANNELINDEX chn = 0; chn < sndFile.GetNumChannels(); chn++)
		{
			state->Chn[chn].Reset(ModChannel::resetTotal, sndFile, chn);
			state->Chn[chn].nOldGlobalVolSlide = 0;
			state->Chn[chn].nOldChnVolSlide = 0;
			state->Chn[chn].nNote = state->Chn[chn].nNewNote = state->Chn[chn].nLastNote = NOTE_NONE;
		}
	}