void CSoundFile::FineVolumeUp(ModChannel *pChn, ModCommand::PARAM param, bool volCol) const
{
	if(GetType() == MOD_TYPE_XM)
	{
		// FT2 compatibility: EAx / EBx memory is not linked
		// Test case: FineVol-LinkMem.xm
		if(param) pChn->nOldFineVolUpDown = (param << 4) | (pChn->nOldFineVolUpDown & 0x0F); else param = (pChn->nOldFineVolUpDown >> 4);
	} else if(volCol)
	{
		if(param) pChn->nOldVolParam = param; else param = pChn->nOldVolParam;
	} else
	{
		if(param) pChn->nOldFineVolUpDown = param; else param = pChn->nOldFineVolUpDown;
	}

	if(pChn->isFirstTick)
	{
		pChn->nVolume += param * 4;
		if(pChn->nVolume > 256) pChn->nVolume = 256;
		if(GetType() & MOD_TYPE_MOD) pChn->dwFlags.set(CHN_FASTVOLRAMP);
	}
}