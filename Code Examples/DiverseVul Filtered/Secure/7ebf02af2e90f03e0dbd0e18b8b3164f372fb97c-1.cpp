void CSoundFile::Tremolo(ModChannel *pChn, uint32 param) const
{
	if (param & 0x0F) pChn->nTremoloDepth = (param & 0x0F) << 2;
	if (param & 0xF0) pChn->nTremoloSpeed = (param >> 4) & 0x0F;
	pChn->dwFlags.set(CHN_TREMOLO);
}