void CSoundFile::NoteSlide(ModChannel *pChn, uint32 param, bool slideUp, bool retrig) const
{
	uint8 x, y;
	if(m_SongFlags[SONG_FIRSTTICK])
	{
		x = param & 0xF0;
		if (x)
			pChn->nNoteSlideSpeed = (x >> 4);
		y = param & 0x0F;
		if (y)
			pChn->nNoteSlideStep = y;
		pChn->nNoteSlideCounter = pChn->nNoteSlideSpeed;
	} else
	{
		if (--pChn->nNoteSlideCounter == 0)
		{
			pChn->nNoteSlideCounter = pChn->nNoteSlideSpeed;
			// update it
			pChn->nPeriod = GetPeriodFromNote
				((slideUp ? 1 : -1)  * pChn->nNoteSlideStep + GetNoteFromPeriod(pChn->nPeriod), 8363, 0);

			if(retrig)
			{
				pChn->position.Set(0);
			}
		}
	}
}