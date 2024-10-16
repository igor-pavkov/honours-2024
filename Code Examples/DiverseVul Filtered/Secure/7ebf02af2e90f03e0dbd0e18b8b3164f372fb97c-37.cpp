void CSoundFile::TonePortamento(ModChannel *pChn, uint32 param) const
{
	pChn->dwFlags.set(CHN_PORTAMENTO);

	//IT compatibility 03: Share effect memory with portamento up/down
	if((!m_SongFlags[SONG_ITCOMPATGXX] && m_playBehaviour[kITPortaMemoryShare]) || GetType() == MOD_TYPE_PLM)
	{
		if(param == 0) param = pChn->nOldPortaUp;
		pChn->nOldPortaUp = pChn->nOldPortaDown = static_cast<uint8>(param);
	}

	if(GetType() == MOD_TYPE_MPT && pChn->pModInstrument && pChn->pModInstrument->pTuning)
	{
		//Behavior: Param tells number of finesteps(or 'fullsteps'(notes) with glissando)
		//to slide per row(not per tick).
		const int32 old_PortamentoTickSlide = (m_PlayState.m_nTickCount != 0) ? pChn->m_PortamentoTickSlide : 0;

		if(param)
			pChn->nPortamentoSlide = param;
		else
			if(pChn->nPortamentoSlide == 0)
				return;


		if((pChn->nPortamentoDest > 0 && pChn->nPortamentoSlide < 0) ||
			(pChn->nPortamentoDest < 0 && pChn->nPortamentoSlide > 0))
			pChn->nPortamentoSlide = -pChn->nPortamentoSlide;

		pChn->m_PortamentoTickSlide = static_cast<int32>((m_PlayState.m_nTickCount + 1.0) * pChn->nPortamentoSlide / m_PlayState.m_nMusicSpeed);

		if(pChn->dwFlags[CHN_GLISSANDO])
		{
			pChn->m_PortamentoTickSlide *= pChn->pModInstrument->pTuning->GetFineStepCount() + 1;
			//With glissando interpreting param as notes instead of finesteps.
		}

		const int32 slide = pChn->m_PortamentoTickSlide - old_PortamentoTickSlide;

		if(mpt::abs(pChn->nPortamentoDest) <= mpt::abs(slide))
		{
			if(pChn->nPortamentoDest != 0)
			{
				pChn->m_PortamentoFineSteps += pChn->nPortamentoDest;
				pChn->nPortamentoDest = 0;
				pChn->m_CalculateFreq = true;
			}
		} else
		{
			pChn->m_PortamentoFineSteps += slide;
			pChn->nPortamentoDest -= slide;
			pChn->m_CalculateFreq = true;
		}

		return;
	} //End candidate MPT behavior.

	bool doPorta = !pChn->isFirstTick || (GetType() & (MOD_TYPE_DBM | MOD_TYPE_669)) || (m_PlayState.m_nMusicSpeed == 1 && m_playBehaviour[kSlidesAtSpeed1]);
	if(GetType() == MOD_TYPE_PLM && param >= 0xF0)
	{
		param -= 0xF0;
		doPorta = pChn->isFirstTick;
	}

	if(param)
	{
		if(GetType() == MOD_TYPE_669)
		{
			param *= 10;
		}
		pChn->nPortamentoSlide = param * 4;
	}

	if(pChn->nPeriod && pChn->nPortamentoDest && doPorta)
	{
		if (pChn->nPeriod < pChn->nPortamentoDest)
		{
			int32 delta = pChn->nPortamentoSlide;
			if(m_SongFlags[SONG_LINEARSLIDES] && GetType() != MOD_TYPE_XM)
			{
				uint32 n = pChn->nPortamentoSlide / 4;
				if (n > 255) n = 255;
				// Return (a*b+c/2)/c - no divide error
				// Table is 65536*2(n/192)
				delta = Util::muldivr(pChn->nPeriod, LinearSlideUpTable[n], 65536) - pChn->nPeriod;
				if (delta < 1) delta = 1;
			}
			pChn->nPeriod += delta;
			if (pChn->nPeriod > pChn->nPortamentoDest) pChn->nPeriod = pChn->nPortamentoDest;
		} else
		if (pChn->nPeriod > pChn->nPortamentoDest)
		{
			int32 delta = -pChn->nPortamentoSlide;
			if(m_SongFlags[SONG_LINEARSLIDES] && GetType() != MOD_TYPE_XM)
			{
				uint32 n = pChn->nPortamentoSlide / 4;
				if (n > 255) n = 255;
				delta = Util::muldivr(pChn->nPeriod, LinearSlideDownTable[n], 65536) - pChn->nPeriod;
				if (delta > -1) delta = -1;
			}
			pChn->nPeriod += delta;
			if (pChn->nPeriod < pChn->nPortamentoDest) pChn->nPeriod = pChn->nPortamentoDest;
		}
	}

	// IT compatibility 23. Portamento with no note
	// ProTracker also disables portamento once the target is reached.
	// Test case: PortaTarget.mod
	if(pChn->nPeriod == pChn->nPortamentoDest && (m_playBehaviour[kITPortaTargetReached] || GetType() == MOD_TYPE_MOD))
		pChn->nPortamentoDest = 0;

}