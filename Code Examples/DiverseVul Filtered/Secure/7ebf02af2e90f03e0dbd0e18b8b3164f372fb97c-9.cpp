void CSoundFile::ReverseSampleOffset(ModChannel &chn, ModCommand::PARAM param) const
{
	if(chn.pModSample != nullptr)
	{
		chn.dwFlags.set(CHN_PINGPONGFLAG);
		chn.dwFlags.reset(CHN_LOOP);
		chn.nLength = chn.pModSample->nLength;	// If there was a loop, extend sample to whole length.
		chn.position.Set((chn.nLength - 1) - std::min<SmpLength>(SmpLength(param) << 8, chn.nLength - 1), 0);
	}
}