void CSoundFile::InvertLoop(ModChannel *pChn)
{
	// EFx implementation for MOD files (PT 1.1A and up: Invert Loop)
	// This effect trashes samples. Thanks to 8bitbubsy for making this work. :)
	if(GetType() != MOD_TYPE_MOD || pChn->nEFxSpeed == 0) return;

	// we obviously also need a sample for this
	ModSample *pModSample = const_cast<ModSample *>(pChn->pModSample);
	if(pModSample == nullptr || !pModSample->HasSampleData() || !pModSample->uFlags[CHN_LOOP] || pModSample->uFlags[CHN_16BIT]) return;

	pChn->nEFxDelay += ModEFxTable[pChn->nEFxSpeed & 0x0F];
	if((pChn->nEFxDelay & 0x80) == 0) return; // only applied if the "delay" reaches 128
	pChn->nEFxDelay = 0;

	if (++pChn->nEFxOffset >= pModSample->nLoopEnd - pModSample->nLoopStart)
		pChn->nEFxOffset = 0;

	// TRASH IT!!! (Yes, the sample!)
	uint8 &sample = mpt::byte_cast<uint8 *>(pModSample->sampleb())[pModSample->nLoopStart + pChn->nEFxOffset];
	sample = ~sample;
	ctrlSmp::PrecomputeLoops(*pModSample, *this, false);
}