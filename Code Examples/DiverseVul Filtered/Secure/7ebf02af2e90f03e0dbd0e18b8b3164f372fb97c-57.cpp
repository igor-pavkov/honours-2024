PLUGINDEX CSoundFile::GetActiveInstrumentPlugin(CHANNELINDEX nChn, PluginMutePriority respectMutes) const
{
	// Unlike channel settings, pModInstrument is copied from the original chan to the NNA chan,
	// so we don't need to worry about finding the master chan.

	PLUGINDEX plug = 0;
	if(m_PlayState.Chn[nChn].pModInstrument != nullptr)
	{
		if(respectMutes == RespectMutes && m_PlayState.Chn[nChn].pModSample && m_PlayState.Chn[nChn].pModSample->uFlags[CHN_MUTE])
		{
			plug = 0;
		} else
		{
			plug = m_PlayState.Chn[nChn].pModInstrument->nMixPlug;
		}
	}
	return plug;
}