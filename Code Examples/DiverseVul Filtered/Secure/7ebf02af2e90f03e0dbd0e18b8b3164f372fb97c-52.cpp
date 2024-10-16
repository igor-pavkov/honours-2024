ROWINDEX CSoundFile::PatternLoop(ModChannel *pChn, uint32 param)
{
	if (param)
	{
		// Loop Repeat
		if(pChn->nPatternLoopCount)
		{
			// There's a loop left
			pChn->nPatternLoopCount--;
			if(!pChn->nPatternLoopCount)
			{
				// IT compatibility 10. Pattern loops (+ same fix for S3M files)
				// When finishing a pattern loop, the next loop without a dedicated SB0 starts on the first row after the previous loop.
				if(m_playBehaviour[kITPatternLoopTargetReset] || (GetType() == MOD_TYPE_S3M))
				{
					pChn->nPatternLoop = m_PlayState.m_nRow + 1;
				}

				return ROWINDEX_INVALID;
			}
		} else
		{
			// First time we get into the loop => Set loop count.

			// IT compatibility 10. Pattern loops (+ same fix for XM / MOD / S3M files)
			if(!m_playBehaviour[kITFT2PatternLoop] && !(GetType() & (MOD_TYPE_MOD | MOD_TYPE_S3M)))
			{
				ModChannel *p = m_PlayState.Chn;
				for(CHANNELINDEX i = 0; i < GetNumChannels(); i++, p++) if (p != pChn)
				{
					// Loop on other channel
					if(p->nPatternLoopCount) return ROWINDEX_INVALID;
				}
			}
			pChn->nPatternLoopCount = static_cast<uint8>(param);
		}
		m_PlayState.m_nNextPatStartRow = pChn->nPatternLoop; // Nasty FT2 E60 bug emulation!
		return pChn->nPatternLoop;
	} else
	{
		// Loop Start
		pChn->nPatternLoop = m_PlayState.m_nRow;
	}
	return ROWINDEX_INVALID;
}