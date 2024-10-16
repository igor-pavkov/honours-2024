void CSoundFile::SetSpeed(PlayState &playState, uint32 param) const
{
#ifdef MODPLUG_TRACKER
	// FT2 appears to be decrementing the tick count before checking for zero,
	// so it effectively counts down 65536 ticks with speed = 0 (song speed is a 16-bit variable in FT2)
	if(GetType() == MOD_TYPE_XM && !param)
	{
		playState.m_nMusicSpeed = uint16_max;
	}
#endif	// MODPLUG_TRACKER
	if(param > 0) playState.m_nMusicSpeed = param;
	if(GetType() == MOD_TYPE_STM && param > 0)
	{
		playState.m_nMusicSpeed = std::max<uint32>(param >> 4u, 1);
		playState.m_nMusicTempo = ConvertST2Tempo(static_cast<uint8>(param));
	}
}