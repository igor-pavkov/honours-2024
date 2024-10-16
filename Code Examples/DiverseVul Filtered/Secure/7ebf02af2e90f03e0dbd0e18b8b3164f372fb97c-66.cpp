void CSoundFile::UpdateTimeSignature()
{
	if(!Patterns.IsValidIndex(m_PlayState.m_nPattern) || !Patterns[m_PlayState.m_nPattern].GetOverrideSignature())
	{
		m_PlayState.m_nCurrentRowsPerBeat = m_nDefaultRowsPerBeat;
		m_PlayState.m_nCurrentRowsPerMeasure = m_nDefaultRowsPerMeasure;
	} else
	{
		m_PlayState.m_nCurrentRowsPerBeat = Patterns[m_PlayState.m_nPattern].GetRowsPerBeat();
		m_PlayState.m_nCurrentRowsPerMeasure = Patterns[m_PlayState.m_nPattern].GetRowsPerMeasure();
	}
}