void CSoundFile::ExtendedChannelEffect(ModChannel *pChn, uint32 param)
{
	// S9x and X9x commands (S3M/XM/IT only)
	if(!m_SongFlags[SONG_FIRSTTICK]) return;
	switch(param & 0x0F)
	{
	// S90: Surround Off
	case 0x00:	pChn->dwFlags.reset(CHN_SURROUND);	break;
	// S91: Surround On
	case 0x01:	pChn->dwFlags.set(CHN_SURROUND); pChn->nPan = 128; break;

	////////////////////////////////////////////////////////////
	// ModPlug Extensions
	// S98: Reverb Off
	case 0x08:
		pChn->dwFlags.reset(CHN_REVERB);
		pChn->dwFlags.set(CHN_NOREVERB);
		break;
	// S99: Reverb On
	case 0x09:
		pChn->dwFlags.reset(CHN_NOREVERB);
		pChn->dwFlags.set(CHN_REVERB);
		break;
	// S9A: 2-Channels surround mode
	case 0x0A:
		m_SongFlags.reset(SONG_SURROUNDPAN);
		break;
	// S9B: 4-Channels surround mode
	case 0x0B:
		m_SongFlags.set(SONG_SURROUNDPAN);
		break;
	// S9C: IT Filter Mode
	case 0x0C:
		m_SongFlags.reset(SONG_MPTFILTERMODE);
		break;
	// S9D: MPT Filter Mode
	case 0x0D:
		m_SongFlags.set(SONG_MPTFILTERMODE);
		break;
	// S9E: Go forward
	case 0x0E:
		pChn->dwFlags.reset(CHN_PINGPONGFLAG);
		break;
	// S9F: Go backward (and set playback position to the end if sample just started)
	case 0x0F:
		if(pChn->position.IsZero() && pChn->nLength && (pChn->rowCommand.IsNote() || !pChn->dwFlags[CHN_LOOP]))
		{
			pChn->position.Set(pChn->nLength - 1, SamplePosition::fractMax);
		}
		pChn->dwFlags.set(CHN_PINGPONGFLAG);
		break;
	}
}