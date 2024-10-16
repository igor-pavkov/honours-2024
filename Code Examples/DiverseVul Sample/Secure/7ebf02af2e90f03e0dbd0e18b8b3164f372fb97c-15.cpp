void CSoundFile::PanningSlide(ModChannel *pChn, ModCommand::PARAM param, bool memory)
{
	if(memory)
	{
		// FT2 compatibility: Use effect memory (lxx and rxx in XM shouldn't use effect memory).
		// Test case: PanSlideMem.xm
		if(param)
			pChn->nOldPanSlide = param;
		else
			param = pChn->nOldPanSlide;
	}

	if((GetType() & (MOD_TYPE_XM | MOD_TYPE_MT2)))
	{
		// XM nibble priority
		if((param & 0xF0) != 0)
		{
			param &= 0xF0;
		} else
		{
			param &= 0x0F;
		}
	}

	int32 nPanSlide = 0;

	if(!(GetType() & (MOD_TYPE_XM | MOD_TYPE_MT2)))
	{
		if (((param & 0x0F) == 0x0F) && (param & 0xF0))
		{
			if(m_SongFlags[SONG_FIRSTTICK])
			{
				param = (param & 0xF0) / 4u;
				nPanSlide = - (int)param;
			}
		} else if (((param & 0xF0) == 0xF0) && (param & 0x0F))
		{
			if(m_SongFlags[SONG_FIRSTTICK])
			{
				nPanSlide = (param & 0x0F) * 4u;
			}
		} else if(!m_SongFlags[SONG_FIRSTTICK])
		{
			if (param & 0x0F)
			{
				// IT compatibility: Ignore slide commands with both nibbles set.
				if(!(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT)) || (param & 0xF0) == 0)
					nPanSlide = (int)((param & 0x0F) * 4u);
			} else
			{
				nPanSlide = -(int)((param & 0xF0) / 4u);
			}
		}
	} else
	{
		if(!m_SongFlags[SONG_FIRSTTICK])
		{
			if (param & 0xF0)
			{
				nPanSlide = (int)((param & 0xF0) / 4u);
			} else
			{
				nPanSlide = -(int)((param & 0x0F) * 4u);
			}
			// FT2 compatibility: FT2's panning slide is like IT's fine panning slide (not as deep)
			if(m_playBehaviour[kFT2PanSlide])
				nPanSlide /= 4;
		}
	}
	if (nPanSlide)
	{
		nPanSlide += pChn->nPan;
		nPanSlide = Clamp(nPanSlide, 0, 256);
		pChn->nPan = nPanSlide;
		pChn->nRestorePanOnNewNote = 0;
	}
}