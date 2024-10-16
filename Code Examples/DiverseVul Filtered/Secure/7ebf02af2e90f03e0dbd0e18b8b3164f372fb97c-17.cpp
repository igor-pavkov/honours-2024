void CSoundFile::SendMIDINote(CHANNELINDEX chn, uint16 note, uint16 volume)
{
#ifndef NO_PLUGINS
	auto &channel = m_PlayState.Chn[chn];
	const ModInstrument *pIns = channel.pModInstrument;
	// instro sends to a midi chan
	if (pIns && pIns->HasValidMIDIChannel())
	{
		PLUGINDEX nPlug = pIns->nMixPlug;
		if ((nPlug) && (nPlug <= MAX_MIXPLUGINS))
		{
			IMixPlugin *pPlug = m_MixPlugins[nPlug-1].pMixPlugin;
			if (pPlug != nullptr)
			{
				pPlug->MidiCommand(GetBestMidiChannel(chn), pIns->nMidiProgram, pIns->wMidiBank, note, volume, chn);
				if(note < NOTE_MIN_SPECIAL)
					channel.nLeftVU = channel.nRightVU = 0xFF;
			}
		}
	}
#endif // NO_PLUGINS
}