IMixPlugin *CSoundFile::GetChannelInstrumentPlugin(CHANNELINDEX chn) const
{
#ifndef NO_PLUGINS
	if(m_PlayState.Chn[chn].dwFlags[CHN_MUTE | CHN_SYNCMUTE])
	{
		// Don't process portamento on muted channels. Note that this might have a side-effect
		// on other channels which trigger notes on the same MIDI channel of the same plugin,
		// as those won't be pitch-bent anymore.
		return nullptr;
	}

	if(m_PlayState.Chn[chn].HasMIDIOutput())
	{
		const ModInstrument *pIns = m_PlayState.Chn[chn].pModInstrument;
		// Instrument sends to a MIDI channel
		if(pIns->nMixPlug != 0 && pIns->nMixPlug <= MAX_MIXPLUGINS)
		{
			return m_MixPlugins[pIns->nMixPlug - 1].pMixPlugin;
		}
	}
#else
	MPT_UNREFERENCED_PARAMETER(chn);
#endif // NO_PLUGINS
	return nullptr;
}