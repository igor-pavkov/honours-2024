void CSoundFile::SetTempo(TEMPO param, bool setFromUI)
{
	const CModSpecifications &specs = GetModSpecifications();

	// Anything lower than the minimum tempo is considered to be a tempo slide
	const TEMPO minTempo = (GetType() == MOD_TYPE_MDL) ? TEMPO(1, 0) : TEMPO(32, 0);

	if(setFromUI)
	{
		// Set tempo from UI - ignore slide commands and such.
		m_PlayState.m_nMusicTempo = Clamp(param, specs.GetTempoMin(), specs.GetTempoMax());
	} else if(param >= minTempo && m_SongFlags[SONG_FIRSTTICK] == !m_playBehaviour[kMODTempoOnSecondTick])
	{
		// ProTracker sets the tempo after the first tick.
		// Note: The case of one tick per row is handled in ProcessRow() instead.
		// Test case: TempoChange.mod
		m_PlayState.m_nMusicTempo = std::min(param, specs.GetTempoMax());
	} else if(param < minTempo && !m_SongFlags[SONG_FIRSTTICK])
	{
		// Tempo Slide
		TEMPO tempDiff(param.GetInt() & 0x0F, 0);
		if((param.GetInt() & 0xF0) == 0x10)
			m_PlayState.m_nMusicTempo += tempDiff;
		else
			m_PlayState.m_nMusicTempo -= tempDiff;

		TEMPO tempoMin = specs.GetTempoMin(), tempoMax = specs.GetTempoMax();
		if(m_playBehaviour[kTempoClamp])	// clamp tempo correctly in compatible mode
		{
			tempoMax.Set(255);
		}
		Limit(m_PlayState.m_nMusicTempo, tempoMin, tempoMax);
	}
}