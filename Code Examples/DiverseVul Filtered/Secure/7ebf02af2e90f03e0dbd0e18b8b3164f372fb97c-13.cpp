void CSoundFile::ChannelVolSlide(ModChannel *pChn, ModCommand::PARAM param) const
{
	int32 nChnSlide = 0;
	if (param) pChn->nOldChnVolSlide = param; else param = pChn->nOldChnVolSlide;

	if (((param & 0x0F) == 0x0F) && (param & 0xF0))
	{
		if(m_SongFlags[SONG_FIRSTTICK]) nChnSlide = param >> 4;
	} else if (((param & 0xF0) == 0xF0) && (param & 0x0F))
	{
		if(m_SongFlags[SONG_FIRSTTICK]) nChnSlide = - (int)(param & 0x0F);
	} else
	{
		if(!m_SongFlags[SONG_FIRSTTICK])
		{
			if (param & 0x0F)
			{
				if(!(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_J2B | MOD_TYPE_DBM)) || (param & 0xF0) == 0)
					nChnSlide = -(int)(param & 0x0F);
			} else
			{
				nChnSlide = (int)((param & 0xF0) >> 4);
			}
		}
	}
	if (nChnSlide)
	{
		nChnSlide += pChn->nGlobalVol;
		nChnSlide = Clamp(nChnSlide, 0, 64);
		pChn->nGlobalVol = nChnSlide;
	}
}