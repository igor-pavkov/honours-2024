void CSoundFile::UpdateS3MEffectMemory(ModChannel *pChn, ModCommand::PARAM param) const
{
	pChn->nOldVolumeSlide = param;	// Dxy / Kxy / Lxy
	pChn->nOldPortaUp = param;		// Exx / Fxx
	pChn->nOldPortaDown = param;	// Exx / Fxx
	pChn->nTremorParam = param;		// Ixy
	pChn->nArpeggio = param;		// Jxy
	pChn->nRetrigParam = param;		// Qxy
	pChn->nTremoloDepth = (param & 0x0F) << 2;	// Rxy
	pChn->nTremoloSpeed = (param >> 4) & 0x0F;	// Rxy
	// Sxy is not handled here.
}