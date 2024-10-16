PLUGINDEX CSoundFile::GetBestPlugin(CHANNELINDEX nChn, PluginPriority priority, PluginMutePriority respectMutes) const
{
	if (nChn >= MAX_CHANNELS)		//Check valid channel number
	{
		return 0;
	}

	//Define search source order
	PLUGINDEX nPlugin = 0;
	switch (priority)
	{
		case ChannelOnly:
			nPlugin = GetChannelPlugin(nChn, respectMutes);
			break;
		case InstrumentOnly:
			nPlugin  = GetActiveInstrumentPlugin(nChn, respectMutes);
			break;
		case PrioritiseInstrument:
			nPlugin  = GetActiveInstrumentPlugin(nChn, respectMutes);
			if ((!nPlugin) || (nPlugin > MAX_MIXPLUGINS))
			{
				nPlugin = GetChannelPlugin(nChn, respectMutes);
			}
			break;
		case PrioritiseChannel:
			nPlugin  = GetChannelPlugin(nChn, respectMutes);
			if ((!nPlugin) || (nPlugin > MAX_MIXPLUGINS))
			{
				nPlugin = GetActiveInstrumentPlugin(nChn, respectMutes);
			}
			break;
	}

	return nPlugin; // 0 Means no plugin found.
}