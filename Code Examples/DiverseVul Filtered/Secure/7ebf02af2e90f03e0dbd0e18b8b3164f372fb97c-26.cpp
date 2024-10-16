void CSoundFile::DoFreqSlide(ModChannel *pChn, int32 nFreqSlide) const
{
	if(!pChn->nPeriod) return;
	if(GetType() == MOD_TYPE_669)
	{
		// Like other oldskool trackers, Composer 669 doesn't have linear slides...
		// But the slides are done in Hertz rather than periods, meaning that they
		// are more effective in the lower notes (rather than the higher notes).
		nFreqSlide *= -20;
	}
	if(m_SongFlags[SONG_LINEARSLIDES] && GetType() != MOD_TYPE_XM)
	{
		// IT Linear slides
		const auto nOldPeriod = pChn->nPeriod;
		uint32 n = mpt::abs(nFreqSlide) / 4u;
		LimitMax(n, 255u);
		if(n != 0)
		{
			pChn->nPeriod = Util::muldivr(pChn->nPeriod, nFreqSlide < 0 ? GetLinearSlideUpTable(this, n) : GetLinearSlideDownTable(this, n), 65536);
			if(pChn->nPeriod == nOldPeriod)
			{
				const bool incPeriod = m_playBehaviour[kHertzInLinearMode] == (nFreqSlide < 0);
				if(incPeriod && pChn->nPeriod < Util::MaxValueOfType(pChn->nPeriod))
					pChn->nPeriod++;
				else if(!incPeriod && pChn->nPeriod > 1)
					pChn->nPeriod--;
			}
		}
	} else
	{
		pChn->nPeriod += nFreqSlide;
	}
	if (pChn->nPeriod < 1)
	{
		pChn->nPeriod = 1;
		if(GetType() == MOD_TYPE_S3M)
		{
			pChn->nFadeOutVol = 0;
			pChn->dwFlags.set(CHN_NOTEFADE | CHN_FASTVOLRAMP);
		}
	}
}