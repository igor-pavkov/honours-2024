CHANNELINDEX CSoundFile::GetNNAChannel(CHANNELINDEX nChn) const
{
	const ModChannel *pChn = &m_PlayState.Chn[nChn];

	// Check for empty channel
	const ModChannel *pi = &m_PlayState.Chn[m_nChannels];
	for (CHANNELINDEX i = m_nChannels; i < MAX_CHANNELS; i++, pi++) if (!pi->nLength) return i;
	if (!pChn->nFadeOutVol) return 0;

	// All channels are used: check for lowest volume
	CHANNELINDEX result = 0;
	uint32 vol = (1u << (14 + 9)) / 4u;	// 25%
	uint32 envpos = uint32_max;
	const ModChannel *pj = &m_PlayState.Chn[m_nChannels];
	for (CHANNELINDEX j = m_nChannels; j < MAX_CHANNELS; j++, pj++)
	{
		if (!pj->nFadeOutVol) return j;
		// Use a combination of real volume [14 bit] (which includes volume envelopes, but also potentially global volume) and note volume [9 bit].
		// Rationale: We need volume envelopes in case e.g. all NNA channels are playing at full volume but are looping on a 0-volume envelope node.
		// But if global volume is not applied to master and the global volume temporarily drops to 0, we would kill arbitrary channels. Hence, add the note volume as well.
		uint32 v = (pj->nRealVolume << 9) | pj->nVolume;
		if(pj->dwFlags[CHN_LOOP]) v >>= 1;
		if ((v < vol) || ((v == vol) && (pj->VolEnv.nEnvPosition > envpos)))
		{
			envpos = pj->VolEnv.nEnvPosition;
			vol = v;
			result = j;
		}
	}
	return result;
}