void CSoundFile::PortamentoDown(CHANNELINDEX nChn, ModCommand::PARAM param, const bool doFinePortamentoAsRegular)
{
	ModChannel *pChn = &m_PlayState.Chn[nChn];

	if(param)
	{
		// FT2 compatibility: Separate effect memory for all portamento commands
		// Test case: Porta-LinkMem.xm
		if(!m_playBehaviour[kFT2PortaUpDownMemory])
			pChn->nOldPortaUp = param;
		pChn->nOldPortaDown = param;
	} else
	{
		param = pChn->nOldPortaDown;
	}

	const bool doFineSlides = !doFinePortamentoAsRegular && !(GetType() & (MOD_TYPE_MOD | MOD_TYPE_XM | MOD_TYPE_MT2 | MOD_TYPE_MED | MOD_TYPE_AMF0 | MOD_TYPE_DIGI | MOD_TYPE_STP | MOD_TYPE_DTM));

	// Process MIDI pitch bend for instrument plugins
	MidiPortamento(nChn, -static_cast<int>(param), doFineSlides);

	if(GetType() == MOD_TYPE_MPT && pChn->pModInstrument && pChn->pModInstrument->pTuning)
	{
		// Portamento for instruments with custom tuning
		if(param >= 0xF0 && !doFinePortamentoAsRegular)
			PortamentoFineMPT(pChn, -static_cast<int>(param - 0xF0));
		else if(param >= 0xE0 && !doFinePortamentoAsRegular)
			PortamentoExtraFineMPT(pChn, -static_cast<int>(param - 0xE0));
		else
			PortamentoMPT(pChn, -static_cast<int>(param));
		return;
	} else if(GetType() == MOD_TYPE_PLM)
	{
		// A normal portamento up or down makes a follow-up tone portamento go the same direction.
		pChn->nPortamentoDest = 65535;
	}

	if(doFineSlides && param >= 0xE0)
	{
		if (param & 0x0F)
		{
			if ((param & 0xF0) == 0xF0)
			{
				FinePortamentoDown(pChn, param & 0x0F);
				return;
			} else if ((param & 0xF0) == 0xE0 && GetType() != MOD_TYPE_DBM)
			{
				ExtraFinePortamentoDown(pChn, param & 0x0F);
				return;
			}
		}
		if(GetType() != MOD_TYPE_DBM)
		{
			// DBM only has fine slides, no extra-fine slides.
			return;
		}
	}

	if(!pChn->isFirstTick || (m_PlayState.m_nMusicSpeed == 1 && m_playBehaviour[kSlidesAtSpeed1]) || GetType() == MOD_TYPE_669)
	{
		DoFreqSlide(pChn, int(param) * 4);
	}
}