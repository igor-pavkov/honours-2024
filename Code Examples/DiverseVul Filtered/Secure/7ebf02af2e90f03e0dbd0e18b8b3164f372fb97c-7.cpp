void CSoundFile::ExtraFinePortamentoDown(ModChannel *pChn, ModCommand::PARAM param) const
{
	if(GetType() == MOD_TYPE_XM)
	{
		// FT2 compatibility: E1x / E2x / X1x / X2x memory is not linked
		// Test case: Porta-LinkMem.xm
		if(param) pChn->nOldExtraFinePortaUpDown = (pChn->nOldExtraFinePortaUpDown & 0xF0) | (param & 0x0F); else param = (pChn->nOldExtraFinePortaUpDown & 0x0F);
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
				int oldPeriod = pChn->nPeriod;
				pChn->nPeriod = Util::muldivr(pChn->nPeriod, GetFineLinearSlideDownTable(this, param & 0x0F), 65536);
				if(oldPeriod == pChn->nPeriod) pChn->nPeriod--;
			} else
			{
				pChn->nPeriod += (int)(param);
				if (pChn->nPeriod > 0xFFFF) pChn->nPeriod = 0xFFFF;
			}
		}
	}
}