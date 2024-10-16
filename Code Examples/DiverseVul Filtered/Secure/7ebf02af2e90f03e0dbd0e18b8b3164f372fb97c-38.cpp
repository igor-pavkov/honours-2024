ROWINDEX CSoundFile::PatternBreak(PlayState &state, CHANNELINDEX chn, uint8 param) const
{
	if(param >= 64 && (GetType() & MOD_TYPE_S3M))
	{
		// ST3 ignores invalid pattern breaks.
		return ROWINDEX_INVALID;
	}

	state.m_nNextPatStartRow = 0; // FT2 E60 bug

	return static_cast<ROWINDEX>(CalculateXParam(state.m_nPattern, state.m_nRow, chn));
}