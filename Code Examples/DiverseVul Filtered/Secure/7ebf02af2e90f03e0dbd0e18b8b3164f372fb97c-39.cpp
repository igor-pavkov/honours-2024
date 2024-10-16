void CSoundFile::MidiPortamento(CHANNELINDEX nChn, int param, bool doFineSlides)
{
	int actualParam = mpt::abs(param);
	int pitchBend = 0;

	// Old MIDI Pitch Bends:
	// - Applied on every tick
	// - No fine pitch slides (they are interpreted as normal slides)
	// New MIDI Pitch Bends:
	// - Behaviour identical to sample pitch bends if the instrument's PWD parameter corresponds to the actual VSTi setting.

	if(doFineSlides && actualParam >= 0xE0 && !m_playBehaviour[kOldMIDIPitchBends])
	{
		if(m_PlayState.Chn[nChn].isFirstTick)
		{
			// Extra fine slide...
			pitchBend = (actualParam & 0x0F) * sgn(param);
			if(actualParam >= 0xF0)
			{
				// ... or just a fine slide!
				pitchBend *= 4;
			}
		}
	} else if(!m_PlayState.Chn[nChn].isFirstTick || m_playBehaviour[kOldMIDIPitchBends])
	{
		// Regular slide
		pitchBend = param * 4;
	}

	if(pitchBend)
	{
#ifndef NO_PLUGINS
		IMixPlugin *plugin = GetChannelInstrumentPlugin(nChn);
		if(plugin != nullptr)
		{
			int8 pwd = 13;	// Early OpenMPT legacy... Actually it's not *exactly* 13, but close enough...
			if(m_PlayState.Chn[nChn].pModInstrument != nullptr)
			{
				pwd = m_PlayState.Chn[nChn].pModInstrument->midiPWD;
			}
			plugin->MidiPitchBend(GetBestMidiChannel(nChn), pitchBend, pwd);
		}
#endif // NO_PLUGINS
	}
}