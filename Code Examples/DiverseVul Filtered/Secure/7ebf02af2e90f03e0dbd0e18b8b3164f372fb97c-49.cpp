void CSoundFile::FineVolumeDown(ModChannel *pChn, ModCommand::PARAM param, bool volCol) const
{
	if(GetType() == MOD_TYPE_XM)
	{
		// FT2 compatibility: EAx / EBx memory is not linked
		// Test case: FineVol-LinkMem.xm
		if(param) pChn->nOldFineVolUpDown = param | (pChn->nOldFineVolUpDown & 0xF0); else param = (pChn->nOldFineVolUpDown & 0x0F);
	} else if(volCol)
	{
		if(param) pChn->nOldVolParam = param; else param = pChn->nOldVolParam;
	} else
	{
		if(param) pChn->nOldFineVolUpDown = param; else param = pChn->nOldFineVolUpDown;
	}

	if(pChn->isFirstTick)
	{
		pChn->nVolume -= param * 4;
		if(pChn->nVolume < 0) pChn->nVolume = 0;
		if(GetType() & MOD_TYPE_MOD) pChn->dwFlags.set(CHN_FASTVOLRAMP);
	}
}