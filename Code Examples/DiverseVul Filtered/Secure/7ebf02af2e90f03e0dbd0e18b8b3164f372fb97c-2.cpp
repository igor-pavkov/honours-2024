CHANNELINDEX CSoundFile::CheckNNA(CHANNELINDEX nChn, uint32 instr, int note, bool forceCut)
{
	CHANNELINDEX nnaChn = CHANNELINDEX_INVALID;
	ModChannel &srcChn = m_PlayState.Chn[nChn];
	const ModInstrument *pIns = nullptr;
	if(!ModCommand::IsNote(static_cast<ModCommand::NOTE>(note)))
	{
		return nnaChn;
	}
	// Always NNA cut - using
	if((!(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_MT2)) || !m_nInstruments || forceCut) && !srcChn.HasMIDIOutput())
	{
		if(!srcChn.nLength || srcChn.dwFlags[CHN_MUTE] || !(srcChn.rightVol | srcChn.leftVol))
		{
			return CHANNELINDEX_INVALID;
		}

		nnaChn = GetNNAChannel(nChn);
		if(!nnaChn) return CHANNELINDEX_INVALID;
		ModChannel &chn = m_PlayState.Chn[nnaChn];
		// Copy Channel
		chn = srcChn;
		chn.dwFlags.reset(CHN_VIBRATO | CHN_TREMOLO | CHN_MUTE | CHN_PORTAMENTO);
		chn.nPanbrelloOffset = 0;
		chn.nMasterChn = nChn + 1;
		chn.nCommand = CMD_NONE;
		chn.rowCommand.Clear();
		// Cut the note
		chn.nFadeOutVol = 0;
		chn.dwFlags.set(CHN_NOTEFADE | CHN_FASTVOLRAMP);
		// Stop this channel
		srcChn.nLength = 0;
		srcChn.position.Set(0);
		srcChn.nROfs = srcChn.nLOfs = 0;
		srcChn.rightVol = srcChn.leftVol = 0;
		return nnaChn;
	}
	if(instr > GetNumInstruments()) instr = 0;
	const ModSample *pSample = srcChn.pModSample;
	// If no instrument is given, assume previous instrument to still be valid.
	// Test case: DNA-NoInstr.it
	pIns = instr > 0 ? Instruments[instr] : srcChn.pModInstrument;
	if(pIns != nullptr)
	{
		uint32 n = pIns->Keyboard[note - NOTE_MIN];
		note = pIns->NoteMap[note - NOTE_MIN];
		if ((n) && (n < MAX_SAMPLES))
		{
			pSample = &Samples[n];
		} else if(m_playBehaviour[kITEmptyNoteMapSlot] && !pIns->HasValidMIDIChannel())
		{
			// Impulse Tracker ignores empty slots.
			// We won't ignore them if a plugin is assigned to this slot, so that VSTis still work as intended.
			// Test case: emptyslot.it, PortaInsNum.it, gxsmp.it, gxsmp2.it
			return CHANNELINDEX_INVALID;
		}
	}
	if (srcChn.dwFlags[CHN_MUTE])
		return CHANNELINDEX_INVALID;

	for(CHANNELINDEX i = nChn; i < MAX_CHANNELS; i++)
	if(i >= m_nChannels || i == nChn)
	{
		ModChannel &chn = m_PlayState.Chn[i];
		bool applyDNAtoPlug = false;
		if((chn.nMasterChn == nChn + 1 || i == nChn) && chn.pModInstrument != nullptr)
		{
			bool bOk = false;
			// Duplicate Check Type
			switch(chn.pModInstrument->nDCT)
			{
			// Note
			case DCT_NOTE:
				if(note && chn.nNote == note && pIns == chn.pModInstrument) bOk = true;
				if(pIns && pIns->nMixPlug) applyDNAtoPlug = true;
				break;
			// Sample
			case DCT_SAMPLE:
				if(pSample != nullptr && pSample == chn.pModSample) bOk = true;
				break;
			// Instrument
			case DCT_INSTRUMENT:
				if(pIns == chn.pModInstrument) bOk = true;
				if(pIns && pIns->nMixPlug) applyDNAtoPlug = true;
				break;
			// Plugin
			case DCT_PLUGIN:
				if(pIns && (pIns->nMixPlug) && (pIns->nMixPlug == chn.pModInstrument->nMixPlug))
				{
					applyDNAtoPlug = true;
					bOk = true;
				}
				break;

			}
			// Duplicate Note Action
			if (bOk)
			{
#ifndef NO_PLUGINS
				if (applyDNAtoPlug && chn.nNote != NOTE_NONE)
				{
					switch(chn.pModInstrument->nDNA)
					{
					case DNA_NOTECUT:
					case DNA_NOTEOFF:
					case DNA_NOTEFADE:
						// Switch off duplicated note played on this plugin
						SendMIDINote(i, chn.GetPluginNote(m_playBehaviour[kITRealNoteMapping]) + NOTE_MAX_SPECIAL, 0);
						chn.nArpeggioLastNote = NOTE_NONE;
						break;
					}
				}
#endif // NO_PLUGINS

				switch(chn.pModInstrument->nDNA)
				{
				// Cut
				case DNA_NOTECUT:
					KeyOff(&chn);
					chn.nVolume = 0;
					break;
				// Note Off
				case DNA_NOTEOFF:
					KeyOff(&chn);
					break;
				// Note Fade
				case DNA_NOTEFADE:
					chn.dwFlags.set(CHN_NOTEFADE);
					break;
				}
				if(!chn.nVolume)
				{
					chn.nFadeOutVol = 0;
					chn.dwFlags.set(CHN_NOTEFADE | CHN_FASTVOLRAMP);
				}
			}
		}
	}

	// Do we need to apply New/Duplicate Note Action to a VSTi?
	bool applyNNAtoPlug = false;
#ifndef NO_PLUGINS
	IMixPlugin *pPlugin = nullptr;
	if(srcChn.HasMIDIOutput() && ModCommand::IsNote(srcChn.nNote)) // instro sends to a midi chan
	{
		PLUGINDEX nPlugin = GetBestPlugin(nChn, PrioritiseInstrument, RespectMutes);

		if(nPlugin > 0 && nPlugin <= MAX_MIXPLUGINS)
		{
			pPlugin =  m_MixPlugins[nPlugin-1].pMixPlugin;
			if(pPlugin)
			{
				// apply NNA to this plugin iff it is currently playing a note on this tracker channel
				// (and if it is playing a note, we know that would be the last note played on this chan).
				applyNNAtoPlug = pPlugin->IsNotePlaying(srcChn.GetPluginNote(m_playBehaviour[kITRealNoteMapping]), GetBestMidiChannel(nChn), nChn);
			}
		}
	}
#endif // NO_PLUGINS

	// New Note Action
	if((srcChn.nRealVolume > 0 && srcChn.nLength > 0) || applyNNAtoPlug)
	{
		nnaChn = GetNNAChannel(nChn);
		if(nnaChn != 0)
		{
			ModChannel &chn = m_PlayState.Chn[nnaChn];
			// Copy Channel
			chn = srcChn;
			chn.dwFlags.reset(CHN_VIBRATO | CHN_TREMOLO | CHN_PORTAMENTO);
			chn.nPanbrelloOffset = 0;

			chn.nMasterChn = nChn < GetNumChannels() ? nChn + 1 : 0;
			chn.nCommand = CMD_NONE;
#ifndef NO_PLUGINS
			if(applyNNAtoPlug && pPlugin)
			{
				//Move note to the NNA channel (odd, but makes sense with DNA stuff).
				//Actually a bad idea since it then become very hard to kill some notes.
				//pPlugin->MoveNote(pChn.nNote, pChn.pModInstrument->nMidiChannel, nChn, n);
				switch(srcChn.nNNA)
				{
				case NNA_NOTEOFF:
				case NNA_NOTECUT:
				case NNA_NOTEFADE:
					//switch off note played on this plugin, on this tracker channel and midi channel
					//pPlugin->MidiCommand(pChn.pModInstrument->nMidiChannel, pChn.pModInstrument->nMidiProgram, pChn.nNote + NOTE_MAX_SPECIAL, 0, n);
					SendMIDINote(nChn, NOTE_KEYOFF, 0);
					srcChn.nArpeggioLastNote = NOTE_NONE;
					break;
				}
			}
#endif // NO_PLUGINS

			// Key Off the note
			switch(srcChn.nNNA)
			{
			case NNA_NOTEOFF:
				KeyOff(&chn);
				break;
			case NNA_NOTECUT:
				chn.nFadeOutVol = 0;
				chn.dwFlags.set(CHN_NOTEFADE);
				break;
			case NNA_NOTEFADE:
				chn.dwFlags.set(CHN_NOTEFADE);
				break;
			}
			if(!chn.nVolume)
			{
				chn.nFadeOutVol = 0;
				chn.dwFlags.set(CHN_NOTEFADE | CHN_FASTVOLRAMP);
			}
			// Stop this channel
			srcChn.nLength = 0;
			srcChn.position.Set(0);
			srcChn.nROfs = srcChn.nLOfs = 0;
		}
	}
	return nnaChn;
}