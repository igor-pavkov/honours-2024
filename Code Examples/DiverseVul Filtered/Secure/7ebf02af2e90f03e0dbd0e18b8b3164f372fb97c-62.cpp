void CSoundFile::Panning(ModChannel *pChn, uint32 param, PanningType panBits) const
{
	// No panning in ProTracker mode
	if(m_playBehaviour[kMODIgnorePanning])
	{
		return;
	}
	// IT Compatibility (and other trackers as well): panning disables surround (unless panning in rear channels is enabled, which is not supported by the original trackers anyway)
	if (!m_SongFlags[SONG_SURROUNDPAN] && (panBits == Pan8bit || m_playBehaviour[kPanOverride]))
	{
		pChn->dwFlags.reset(CHN_SURROUND);
	}
	if(panBits == Pan4bit)
	{
		// 0...15 panning
		pChn->nPan = (param * 256 + 8) / 15;
	} else if(panBits == Pan6bit)
	{
		// 0...64 panning
		if(param > 64) param = 64;
		pChn->nPan = param * 4;
	} else
	{
		if(!(GetType() & (MOD_TYPE_S3M | MOD_TYPE_DSM | MOD_TYPE_AMF | MOD_TYPE_MTM)))
		{
			// Real 8-bit panning
			pChn->nPan = param;
		} else
		{
			// 7-bit panning + surround
			if(param <= 0x80)
			{
				pChn->nPan = param << 1;
			} else if(param == 0xA4)
			{
				pChn->dwFlags.set(CHN_SURROUND);
				pChn->nPan = 0x80;
			}
		}
	}

	pChn->dwFlags.set(CHN_FASTVOLRAMP);
	pChn->nRestorePanOnNewNote = 0;
	//IT compatibility 20. Set pan overrides random pan
	if(m_playBehaviour[kPanOverride])
	{
		pChn->nPanSwing = 0;
		pChn->nPanbrelloOffset = 0;
	}
}