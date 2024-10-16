void CSoundFile::VolumeSlide(ModChannel *pChn, ModCommand::PARAM param)
{
	if (param)
		pChn->nOldVolumeSlide = param;
	else
		param = pChn->nOldVolumeSlide;

	if((GetType() & (MOD_TYPE_MOD | MOD_TYPE_XM | MOD_TYPE_MT2 | MOD_TYPE_MED | MOD_TYPE_DIGI | MOD_TYPE_STP | MOD_TYPE_DTM)))
	{
		// MOD / XM nibble priority
		if((param & 0xF0) != 0)
		{
			param &= 0xF0;
		} else
		{
			param &= 0x0F;
		}
	}

	int newvolume = pChn->nVolume;
	if(!(GetType() & (MOD_TYPE_MOD | MOD_TYPE_XM | MOD_TYPE_AMF0 | MOD_TYPE_MED | MOD_TYPE_DIGI)))
	{
		if ((param & 0x0F) == 0x0F) //Fine upslide or slide -15
		{
			if (param & 0xF0) //Fine upslide
			{
				FineVolumeUp(pChn, (param >> 4), false);
				return;
			} else //Slide -15
			{
				if(pChn->isFirstTick && !m_SongFlags[SONG_FASTVOLSLIDES])
				{
					newvolume -= 0x0F * 4;
				}
			}
		} else
		if ((param & 0xF0) == 0xF0) //Fine downslide or slide +15
		{
			if (param & 0x0F) //Fine downslide
			{
				FineVolumeDown(pChn, (param & 0x0F), false);
				return;
			} else //Slide +15
			{
				if(pChn->isFirstTick && !m_SongFlags[SONG_FASTVOLSLIDES])
				{
					newvolume += 0x0F * 4;
				}
			}
		}
	}
	if(!pChn->isFirstTick || m_SongFlags[SONG_FASTVOLSLIDES] || (m_PlayState.m_nMusicSpeed == 1 && GetType() == MOD_TYPE_DBM))
	{
		// IT compatibility: Ignore slide commands with both nibbles set.
		if (param & 0x0F)
		{
			if(!(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT)) || (param & 0xF0) == 0)
				newvolume -= (int)((param & 0x0F) * 4);
		}
		else
		{
			newvolume += (int)((param & 0xF0) >> 2);
		}
		if (GetType() == MOD_TYPE_MOD) pChn->dwFlags.set(CHN_FASTVOLRAMP);
	}
	newvolume = Clamp(newvolume, 0, 256);

	pChn->nVolume = newvolume;
}