void CSoundFile::FinePortamentoUp(ModChannel *pChn, ModCommand::PARAM param) const
{
	if(GetType() == MOD_TYPE_XM)
	{
		// FT2 compatibility: E1x / E2x / X1x / X2x memory is not linked
		// Test case: Porta-LinkMem.xm
		if(param) pChn->nOldFinePortaUpDown = (pChn->nOldFinePortaUpDown & 0x0F) | (param << 4); else param = (pChn->nOldFinePortaUpDown >> 4);
	} else if(GetType() == MOD_TYPE_MT2)
	{
		if(param) pChn->nOldFinePortaUpDown = param; else param = pChn->nOldFinePortaUpDown;
	}

	if(pChn->isFirstTick)
	{
		if ((pChn->nPeriod) && (param))
		{
			if(m_SongFlags[SONG_LINEARSLIDES] && GetType() != MOD_TYPE_XM)
			{
				const auto oldPeriod = pChn->nPeriod;
				pChn->nPeriod = Util::muldivr(pChn->nPeriod, GetLinearSlideUpTable(this, param & 0x0F), 65536);
				if(oldPeriod == pChn->nPeriod)
				{
					if(m_playBehaviour[kHertzInLinearMode] && pChn->nPeriod < Util::MaxValueOfType(pChn->nPeriod))
						pChn->nPeriod++;
					else if(!m_playBehaviour[kHertzInLinearMode] && pChn->nPeriod > 1)
						pChn->nPeriod--;
				}
			} else
			{
				pChn->nPeriod -= (int)(param * 4);
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
		}
	}
}