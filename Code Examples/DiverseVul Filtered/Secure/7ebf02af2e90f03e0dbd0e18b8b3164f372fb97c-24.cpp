uint8 CSoundFile::GetBestMidiChannel(CHANNELINDEX nChn) const
{
	if(nChn >= MAX_CHANNELS)
	{
		return 0;
	}

	const ModInstrument *ins = m_PlayState.Chn[nChn].pModInstrument;
	if(ins != nullptr)
	{
		if(ins->nMidiChannel == MidiMappedChannel)
		{
			// For mapped channels, return their pattern channel, modulo 16 (because there are only 16 MIDI channels)
			return (m_PlayState.Chn[nChn].nMasterChn ? (m_PlayState.Chn[nChn].nMasterChn - 1) : nChn) % 16;
		} else if(ins->HasValidMIDIChannel())
		{
			return (ins->nMidiChannel - 1) & 0x0F;
		}
	}
	return 0;
}