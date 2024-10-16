uint32 CSoundFile::GetNoteFromPeriod(uint32 period, int32 nFineTune, uint32 nC5Speed) const
{
	if(!period) return 0;
	if(m_playBehaviour[kFT2Periods])
	{
		// FT2's "RelocateTon" function actually rounds up and down, while GetNoteFromPeriod normally just truncates.
		nFineTune += 64;
	}
	// This essentially implements std::lower_bound, with the difference that we don't need an iterable container.
	uint32 minNote = NOTE_MIN, maxNote = NOTE_MAX, count = maxNote - minNote + 1;
	const bool periodIsFreq = PeriodsAreFrequencies();
	while(count > 0)
	{
		const uint32 step = count / 2, midNote = minNote + step;
		uint32 n = GetPeriodFromNote(midNote, nFineTune, nC5Speed);
		if((n > period && !periodIsFreq) || (n < period && periodIsFreq) || !n)
		{
			minNote = midNote + 1;
			count -= step + 1;
		} else
		{
			count = step;
		}
	}
	return minNote;
}