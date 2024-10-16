void CSoundFile::InstrumentChange(ModChannel *pChn, uint32 instr, bool bPorta, bool bUpdVol, bool bResetEnv) const
{
	const ModInstrument *pIns = instr <= GetNumInstruments() ? Instruments[instr] : nullptr;
	const ModSample *pSmp = &Samples[instr];
	ModCommand::NOTE note = pChn->nNewNote;

	if(note == NOTE_NONE && m_playBehaviour[kITInstrWithoutNote]) return;

	if(pIns != nullptr && ModCommand::IsNote(note))
	{
		// Impulse Tracker ignores empty slots.
		// We won't ignore them if a plugin is assigned to this slot, so that VSTis still work as intended.
		// Test case: emptyslot.it, PortaInsNum.it, gxsmp.it, gxsmp2.it
		if(pIns->Keyboard[note - NOTE_MIN] == 0 && m_playBehaviour[kITEmptyNoteMapSlot] && !pIns->HasValidMIDIChannel())
		{
			pChn->pModInstrument = pIns;
			return;
		}

		if(pIns->NoteMap[note - NOTE_MIN] > NOTE_MAX) return;
		uint32 n = pIns->Keyboard[note - NOTE_MIN];
		pSmp = ((n) && (n < MAX_SAMPLES)) ? &Samples[n] : nullptr;
	} else if(GetNumInstruments())
	{
		// No valid instrument, or not a valid note.
		if (note >= NOTE_MIN_SPECIAL) return;
		if(m_playBehaviour[kITEmptyNoteMapSlot] && (pIns == nullptr || !pIns->HasValidMIDIChannel()))
		{
			// Impulse Tracker ignores empty slots.
			// We won't ignore them if a plugin is assigned to this slot, so that VSTis still work as intended.
			// Test case: emptyslot.it, PortaInsNum.it, gxsmp.it, gxsmp2.it
			pChn->pModInstrument = nullptr;
			pChn->nNewIns = 0;
			return;
		}
		pSmp = nullptr;
	}

	bool returnAfterVolumeAdjust = false;

	// instrumentChanged is used for IT carry-on env option
	bool instrumentChanged = (pIns != pChn->pModInstrument);
	const bool sampleChanged = (pChn->pModSample != nullptr) && (pSmp != pChn->pModSample);

	const bool newTuning = (GetType() == MOD_TYPE_MPT && pIns && pIns->pTuning);
	// Playback behavior change for MPT: With portamento don't change sample if it is in
	// the same instrument as previous sample.
	if(bPorta && newTuning && pIns == pChn->pModInstrument && sampleChanged)
		return;

	if(sampleChanged && bPorta)
	{
		// IT compatibility: No sample change (also within multi-sample instruments) during portamento when using Compatible Gxx.
		// Test case: PortaInsNumCompat.it, PortaSampleCompat.it, PortaCutCompat.it
		if(m_playBehaviour[kITPortamentoInstrument] && m_SongFlags[SONG_ITCOMPATGXX] && !pChn->increment.IsZero())
		{
			pSmp = pChn->pModSample;
		}

		// Special XM hack (also applies to MOD / S3M, except when playing IT-style S3Ms, such as k_vision.s3m)
		// Test case: PortaSmpChange.mod, PortaSmpChange.s3m
		if((!instrumentChanged && (GetType() & (MOD_TYPE_XM | MOD_TYPE_MT2)) && pIns)
			|| (GetType() == MOD_TYPE_PLM)
			|| (GetType() == MOD_TYPE_MOD && pChn->IsSamplePlaying())
			|| m_playBehaviour[kST3PortaSampleChange])
		{
			// FT2 doesn't change the sample in this case,
			// but still uses the sample info from the old one (bug?)
			returnAfterVolumeAdjust = true;
		}
	}
	// IT compatibility: A lone instrument number should only reset sample properties to those of the corresponding sample in instrument mode.
	// C#5 01 ... <-- sample 1
	// C-5 .. g02 <-- sample 2
	// ... 01 ... <-- still sample 1, but with properties of sample 2
	// In the above example, no sample change happens on the second row. In the third row, sample 1 keeps playing but with the
	// volume and panning properties of sample 2.
	// Test case: InstrAfterMultisamplePorta.it
	if(m_nInstruments && !instrumentChanged && sampleChanged && pChn->pCurrentSample != nullptr && m_playBehaviour[kITMultiSampleInstrumentNumber] && !pChn->rowCommand.IsNote())
	{
		returnAfterVolumeAdjust = true;
	}

	// IT Compatibility: Envelope pickup after SCx cut (but don't do this when working with plugins, or else envelope carry stops working)
	// Test case: cut-carry.it
	if(!pChn->IsSamplePlaying() && (GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT)) && (!pIns || !pIns->HasValidMIDIChannel()))
	{
		instrumentChanged = true;
	}

	// FT2 compatibility: new instrument + portamento = ignore new instrument number, but reload old instrument settings (the world of XM is upside down...)
	// And this does *not* happen if volume column portamento is used together with note delay... (handled in ProcessEffects(), where all the other note delay stuff is.)
	// Test case: porta-delay.xm
	if(instrumentChanged && bPorta && m_playBehaviour[kFT2PortaIgnoreInstr] && (pChn->pModInstrument != nullptr || pChn->pModSample != nullptr))
	{
		pIns = pChn->pModInstrument;
		pSmp = pChn->pModSample;
		instrumentChanged = false;
	} else
	{
		pChn->pModInstrument = pIns;
	}

	// Update Volume
	if (bUpdVol && (!(GetType() & (MOD_TYPE_MOD | MOD_TYPE_S3M)) || ((pSmp != nullptr && pSmp->HasSampleData()) || pChn->HasMIDIOutput())))
	{
		if(pSmp)
		{
			if(!pSmp->uFlags[SMP_NODEFAULTVOLUME])
				pChn->nVolume = pSmp->nVolume;
		} else if(pIns && pIns->nMixPlug)
		{
			pChn->nVolume = pChn->GetVSTVolume();
		} else
		{
			pChn->nVolume = 0;
		}
	}

	if(returnAfterVolumeAdjust && sampleChanged && m_playBehaviour[kMODSampleSwap] && pSmp != nullptr)
	{
		// ProTracker applies new instrument's finetune but keeps the old sample playing.
		// Test case: PortaSwapPT.mod
		pChn->nFineTune = pSmp->nFineTune;
	}

	if(returnAfterVolumeAdjust) return;


	// Instrument adjust
	pChn->nNewIns = 0;

	// IT Compatiblity: NNA is reset on every note change, not every instrument change (fixes s7xinsnum.it).
	if (pIns && ((!m_playBehaviour[kITNNAReset] && pSmp) || pIns->nMixPlug))
		pChn->nNNA = pIns->nNNA;

	// Update volume
	pChn->UpdateInstrumentVolume(pSmp, pIns);

	// Update panning
	// FT2 compatibility: Only reset panning on instrument numbers, not notes (bUpdVol condition)
	// Test case: PanMemory.xm
	// IT compatibility: Sample and instrument panning is only applied on note change, not instrument change
	// Test case: PanReset.it
	if((bUpdVol || !(GetType() & (MOD_TYPE_XM | MOD_TYPE_MT2))) && !m_playBehaviour[kITPanningReset])
	{
		ApplyInstrumentPanning(pChn, pIns, pSmp);
	}

	// Reset envelopes
	if(bResetEnv)
	{
		// Blurb by Storlek (from the SchismTracker code):
		// Conditions experimentally determined to cause envelope reset in Impulse Tracker:
		// - no note currently playing (of course)
		// - note given, no portamento
		// - instrument number given, portamento, compat gxx enabled
		// - instrument number given, no portamento, after keyoff, old effects enabled
		// If someone can enlighten me to what the logic really is here, I'd appreciate it.
		// Seems like it's just a total mess though, probably to get XMs to play right.

		bool reset, resetAlways;

		// IT Compatibility: Envelope reset
		// Test case: EnvReset.it
		if(m_playBehaviour[kITEnvelopeReset])
		{
			const bool insNumber = (instr != 0);
			reset = (!pChn->nLength
				|| (insNumber && bPorta && m_SongFlags[SONG_ITCOMPATGXX])
				|| (insNumber && !bPorta && pChn->dwFlags[CHN_NOTEFADE | CHN_KEYOFF] && m_SongFlags[SONG_ITOLDEFFECTS]));
			// NOTE: IT2.14 with SB/GUS/etc. output is different. We are going after IT's WAV writer here.
			// For SB/GUS/etc. emulation, envelope carry should only apply when the NNA isn't set to "Note Cut".
			// Test case: CarryNNA.it
			resetAlways = (!pChn->nFadeOutVol || instrumentChanged || pChn->dwFlags[CHN_KEYOFF]);
		} else
		{
			reset = (!bPorta || !(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_DBM)) || m_SongFlags[SONG_ITCOMPATGXX]
				|| !pChn->nLength || (pChn->dwFlags[CHN_NOTEFADE] && !pChn->nFadeOutVol));
			resetAlways = !(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_DBM)) || instrumentChanged || pIns == nullptr || pChn->dwFlags[CHN_KEYOFF | CHN_NOTEFADE];
		}

		if(reset)
		{
			pChn->dwFlags.set(CHN_FASTVOLRAMP);
			if(pIns != nullptr)
			{
				if(resetAlways)
				{
					pChn->ResetEnvelopes();
				} else
				{
					if(!pIns->VolEnv.dwFlags[ENV_CARRY]) pChn->VolEnv.Reset();
					if(!pIns->PanEnv.dwFlags[ENV_CARRY]) pChn->PanEnv.Reset();
					if(!pIns->PitchEnv.dwFlags[ENV_CARRY]) pChn->PitchEnv.Reset();
				}
			}

			// IT Compatibility: Autovibrato reset
			if(!m_playBehaviour[kITVibratoTremoloPanbrello])
			{
				pChn->nAutoVibDepth = 0;
				pChn->nAutoVibPos = 0;
			}
		} else if(pIns != nullptr && !pIns->VolEnv.dwFlags[ENV_ENABLED])
		{
			if(m_playBehaviour[kITPortamentoInstrument])
			{
				pChn->VolEnv.Reset();
			} else
			{
				pChn->ResetEnvelopes();
			}
		}
	}
	// Invalid sample ?
	if(pSmp == nullptr && (pIns == nullptr || !pIns->HasValidMIDIChannel()))
	{
		pChn->pModSample = nullptr;
		pChn->nInsVol = 0;
		return;
	}

	// Tone-Portamento doesn't reset the pingpong direction flag
	if(bPorta && pSmp == pChn->pModSample && pSmp != nullptr)
	{
		// If channel length is 0, we cut a previous sample using SCx. In that case, we have to update sample length, loop points, etc...
		if(GetType() & (MOD_TYPE_S3M|MOD_TYPE_IT|MOD_TYPE_MPT) && pChn->nLength != 0) return;
		pChn->dwFlags.reset(CHN_KEYOFF | CHN_NOTEFADE);
		pChn->dwFlags = (pChn->dwFlags & (CHN_CHANNELFLAGS | CHN_PINGPONGFLAG));
	} else //if(!instrumentChanged || pChn->rowCommand.instr != 0 || !IsCompatibleMode(TRK_FASTTRACKER2))	// SampleChange.xm?
	{
		pChn->dwFlags.reset(CHN_KEYOFF | CHN_NOTEFADE);

		// IT compatibility tentative fix: Don't change bidi loop direction when
		// no sample nor instrument is changed.
		if((m_playBehaviour[kITPingPongNoReset] || !(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT))) && pSmp == pChn->pModSample && !instrumentChanged)
			pChn->dwFlags = (pChn->dwFlags & (CHN_CHANNELFLAGS | CHN_PINGPONGFLAG));
		else
			pChn->dwFlags = (pChn->dwFlags & CHN_CHANNELFLAGS);

		if(pIns)
		{
			// Copy envelope flags (we actually only need the "enabled" and "pitch" flag)
			pChn->VolEnv.flags = pIns->VolEnv.dwFlags;
			pChn->PanEnv.flags = pIns->PanEnv.dwFlags;
			pChn->PitchEnv.flags = pIns->PitchEnv.dwFlags;

			// A cutoff frequency of 0 should not be reset just because the filter envelope is enabled.
			// Test case: FilterEnvReset.it
			if((pIns->PitchEnv.dwFlags & (ENV_ENABLED | ENV_FILTER)) == (ENV_ENABLED | ENV_FILTER) && !m_playBehaviour[kITFilterBehaviour])
			{
				if(!pChn->nCutOff) pChn->nCutOff = 0x7F;
			}

			if(pIns->IsCutoffEnabled()) pChn->nCutOff = pIns->GetCutoff();
			if(pIns->IsResonanceEnabled()) pChn->nResonance = pIns->GetResonance();
		}
	}

	if(pSmp == nullptr)
	{
		pChn->pModSample = nullptr;
		pChn->nLength = 0;
		return;
	}

	if(bPorta && pChn->nLength == 0 && (m_playBehaviour[kFT2PortaNoNote] || m_playBehaviour[kITPortaNoNote]))
	{
		// IT/FT2 compatibility: If the note just stopped on the previous tick, prevent it from restarting.
		// Test cases: PortaJustStoppedNote.xm, PortaJustStoppedNote.it
		pChn->increment.Set(0);
	}

	pChn->pModSample = pSmp;
	pChn->nLength = pSmp->nLength;
	pChn->nLoopStart = pSmp->nLoopStart;
	pChn->nLoopEnd = pSmp->nLoopEnd;
	// ProTracker "oneshot" loops (if loop start is 0, play the whole sample once and then repeat until loop end)
	if(m_playBehaviour[kMODOneShotLoops] && pChn->nLoopStart == 0) pChn->nLoopEnd = pSmp->nLength;
	pChn->dwFlags |= (pSmp->uFlags & (CHN_SAMPLEFLAGS | CHN_SURROUND));

	// IT Compatibility: Autovibrato reset
	if(m_playBehaviour[kITVibratoTremoloPanbrello])
	{
		pChn->nAutoVibDepth = 0;
		pChn->nAutoVibPos = 0;
	}

	if(newTuning)
	{
		pChn->nC5Speed = pSmp->nC5Speed;
		pChn->m_CalculateFreq = true;
		pChn->nFineTune = 0;
	} else if(!bPorta || sampleChanged || !(GetType() & (MOD_TYPE_MOD | MOD_TYPE_XM)))
	{
		// Don't reset finetune changed by "set finetune" command.
		// Test case: finetune.xm, finetune.mod
		// But *do* change the finetune if we switch to a different sample, to fix
		// Miranda`s axe by Jamson (jam007.xm) - this file doesn't use compatible play mode,
		// so we may want to use IsCompatibleMode instead if further problems arise.
		pChn->nC5Speed = pSmp->nC5Speed;
		pChn->nFineTune = pSmp->nFineTune;
	}


	pChn->nTranspose = pSmp->RelativeTone;

	// FT2 compatibility: Don't reset portamento target with new instrument numbers.
	// Test case: Porta-Pickup.xm
	// ProTracker does the same.
	// Test case: PortaTarget.mod
	if(!m_playBehaviour[kFT2PortaTargetNoReset] && GetType() != MOD_TYPE_MOD)
	{
		pChn->nPortamentoDest = 0;
	}
	pChn->m_PortamentoFineSteps = 0;

	if(pChn->dwFlags[CHN_SUSTAINLOOP])
	{
		pChn->nLoopStart = pSmp->nSustainStart;
		pChn->nLoopEnd = pSmp->nSustainEnd;
		if(pChn->dwFlags[CHN_PINGPONGSUSTAIN]) pChn->dwFlags.set(CHN_PINGPONGLOOP);
		pChn->dwFlags.set(CHN_LOOP);
	}
	if(pChn->dwFlags[CHN_LOOP] && pChn->nLoopEnd < pChn->nLength) pChn->nLength = pChn->nLoopEnd;

	// Fix sample position on instrument change. This is needed for IT "on the fly" sample change.
	// XXX is this actually called? In ProcessEffects(), a note-on effect is emulated if there's an on the fly sample change!
	if(pChn->position.GetUInt() >= pChn->nLength)
	{
		if((GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT)))
		{
			pChn->position.Set(0);
		}
	}
}