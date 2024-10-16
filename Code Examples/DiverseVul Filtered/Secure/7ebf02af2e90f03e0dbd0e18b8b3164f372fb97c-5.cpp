void CSoundFile::HandlePatternTransitionEvents()
{
	// MPT sequence override
	if(m_PlayState.m_nSeqOverride != ORDERINDEX_INVALID && m_PlayState.m_nSeqOverride < Order().size())
	{
		if(m_SongFlags[SONG_PATTERNLOOP])
		{
			m_PlayState.m_nPattern = Order()[m_PlayState.m_nSeqOverride];
		}
		m_PlayState.m_nCurrentOrder = m_PlayState.m_nSeqOverride;
		m_PlayState.m_nSeqOverride = ORDERINDEX_INVALID;
	}

	// Channel mutes
	for (CHANNELINDEX chan = 0; chan < GetNumChannels(); chan++)
	{
		if (m_bChannelMuteTogglePending[chan])
		{
			if(GetpModDoc())
			{
				GetpModDoc()->MuteChannel(chan, !GetpModDoc()->IsChannelMuted(chan));
			}
			m_bChannelMuteTogglePending[chan] = false;
		}
	}
}