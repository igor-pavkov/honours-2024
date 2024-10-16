void CSoundFile::GlobalVolSlide(ModCommand::PARAM param, uint8 &nOldGlobalVolSlide)
{
	int32 nGlbSlide = 0;
	if (param) nOldGlobalVolSlide = param; else param = nOldGlobalVolSlide;

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

	if (((param & 0x0F) == 0x0F) && (param & 0xF0))
	{
		if(m_SongFlags[SONG_FIRSTTICK]) nGlbSlide = (param >> 4) * 2;
	} else
	if (((param & 0xF0) == 0xF0) && (param & 0x0F))
	{
		if(m_SongFlags[SONG_FIRSTTICK]) nGlbSlide = - (int)((param & 0x0F) * 2);
	} else
	{
		if(!m_SongFlags[SONG_FIRSTTICK])
		{
			if (param & 0xF0)
			{
				// IT compatibility: Ignore slide commands with both nibbles set.
				if(!(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_IMF | MOD_TYPE_J2B | MOD_TYPE_MID | MOD_TYPE_AMS | MOD_TYPE_AMS2 | MOD_TYPE_DBM)) || (param & 0x0F) == 0)
					nGlbSlide = (int)((param & 0xF0) >> 4) * 2;
			} else
			{
				nGlbSlide = -(int)((param & 0x0F) * 2);
			}
		}
	}
	if (nGlbSlide)
	{
		if(!(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_IMF | MOD_TYPE_J2B | MOD_TYPE_MID | MOD_TYPE_AMS | MOD_TYPE_AMS2 | MOD_TYPE_DBM))) nGlbSlide *= 2;
		nGlbSlide += m_PlayState.m_nGlobalVolume;
		Limit(nGlbSlide, 0, 256);
		m_PlayState.m_nGlobalVolume = nGlbSlide;
	}
}