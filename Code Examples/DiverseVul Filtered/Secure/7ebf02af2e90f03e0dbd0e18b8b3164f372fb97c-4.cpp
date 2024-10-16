void CSoundFile::NoteCut(CHANNELINDEX nChn, uint32 nTick, bool cutSample)
{
	if (m_PlayState.m_nTickCount == nTick)
	{
		ModChannel *pChn = &m_PlayState.Chn[nChn];
		if(cutSample)
		{
			pChn->increment.Set(0);
			pChn->nFadeOutVol = 0;
			pChn->dwFlags.set(CHN_NOTEFADE);
		} else
		{
			pChn->nVolume = 0;
		}
		pChn->dwFlags.set(CHN_FASTVOLRAMP);

		// instro sends to a midi chan
		SendMIDINote(nChn, /*pChn->nNote+*/NOTE_MAX_SPECIAL, 0);
	}
}