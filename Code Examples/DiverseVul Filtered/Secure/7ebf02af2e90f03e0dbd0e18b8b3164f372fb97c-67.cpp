void CSoundFile::NoteChange(ModChannel *pChn, int note, bool bPorta, bool bResetEnv, bool bManual) const
{
	if (note < NOTE_MIN) return;
	const ModSample *pSmp = pChn->pModSample;
	const ModInstrument *pIns = pChn->pModInstrument;

	const bool newTuning = (GetType() == MOD_TYPE_MPT && pIns != nullptr && pIns->pTuning);
	// save the note that's actually used, as it's necessary to properly calculate PPS and stuff
	const int realnote = note;

	if((pIns) && (note - NOTE_MIN < (int)CountOf(pIns->Keyboard)))
	{
		uint32 n = pIns->Keyboard[note - NOTE_MIN];
		if((n) && (n < MAX_SAMPLES))
		{
			pSmp = &Samples[n];
		} else if(m_playBehaviour[kITEmptyNoteMapSlot] && !pChn->HasMIDIOutput())
		{
			// Impulse Tracker ignores empty slots.
			// We won't ignore them if a plugin is assigned to this slot, so that VSTis still work as intended.
			// Test case: emptyslot.it, PortaInsNum.it, gxsmp.it, gxsmp2.it
			return;
		}
		note = pIns->NoteMap[note - NOTE_MIN];
	}
	// Key Off
	if(note > NOTE_MAX)
	{
		// Key Off (+ Invalid Note for XM - TODO is this correct?)
		if(note == NOTE_KEYOFF || !(GetType() & (MOD_TYPE_IT|MOD_TYPE_MPT)))
		{
			KeyOff(pChn);
		} else // Invalid Note -> Note Fade
		{
			if(/*note == NOTE_FADE && */ GetNumInstruments())
				pChn->dwFlags.set(CHN_NOTEFADE);
		}

		// Note Cut
		if (note == NOTE_NOTECUT)
		{
			pChn->dwFlags.set(CHN_NOTEFADE | CHN_FASTVOLRAMP);
			// IT compatibility: Stopping sample playback by setting sample increment to 0 rather than volume
			// Test case: NoteOffInstr.it
			if ((!(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT))) || (m_nInstruments != 0 && !m_playBehaviour[kITInstrWithNoteOff])) pChn->nVolume = 0;
			if(m_playBehaviour[kITInstrWithNoteOff]) pChn->increment.Set(0);
			pChn->nFadeOutVol = 0;
		}

		// IT compatibility tentative fix: Clear channel note memory.
		if(m_playBehaviour[kITClearOldNoteAfterCut])
		{
			pChn->nNote = pChn->nNewNote = NOTE_NONE;
		}
		return;
	}

	if(newTuning)
	{
		if(!bPorta || pChn->nNote == NOTE_NONE)
			pChn->nPortamentoDest = 0;
		else
		{
			pChn->nPortamentoDest = pIns->pTuning->GetStepDistance(pChn->nNote, pChn->m_PortamentoFineSteps, static_cast<Tuning::NOTEINDEXTYPE>(note), 0);
			//Here pChn->nPortamentoDest means 'steps to slide'.
			pChn->m_PortamentoFineSteps = -pChn->nPortamentoDest;
		}
	}

	if(!bPorta && (GetType() & (MOD_TYPE_XM | MOD_TYPE_MED | MOD_TYPE_MT2)))
	{
		if(pSmp)
		{
			pChn->nTranspose = pSmp->RelativeTone;
			pChn->nFineTune = pSmp->nFineTune;
		}
	}
	// IT Compatibility: Update multisample instruments frequency even if instrument is not specified (fixes the guitars in spx-shuttledeparture.it)
	// Test case: freqreset-noins.it
	if(!bPorta && pSmp && m_playBehaviour[kITMultiSampleBehaviour])
		pChn->nC5Speed = pSmp->nC5Speed;

	if(bPorta && !pChn->IsSamplePlaying())
	{
		if(m_playBehaviour[kFT2PortaNoNote])
		{
			// FT2 Compatibility: Ignore notes with portamento if there was no note playing.
			// Test case: 3xx-no-old-samp.xm
			pChn->nPeriod = 0;
			return;
		} else if(m_playBehaviour[kITPortaNoNote])
		{
			// IT Compatibility: Ignore portamento command if no note was playing (e.g. if a previous note has faded out).
			// Test case: Fade-Porta.it
			bPorta = false;
		}
	}

	if(GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2|MOD_TYPE_MED|MOD_TYPE_MOD))
	{
		note += pChn->nTranspose;
		// RealNote = PatternNote + RelativeTone; (0..118, 0 = C-0, 118 = A#9)
		Limit(note, NOTE_MIN + 11, NOTE_MIN + 130);	// 119 possible notes
	} else
	{
		Limit(note, NOTE_MIN, NOTE_MAX);
	}
	if(m_playBehaviour[kITRealNoteMapping])
	{
		// need to memorize the original note for various effects (e.g. PPS)
		pChn->nNote = static_cast<ModCommand::NOTE>(Clamp(realnote, NOTE_MIN, NOTE_MAX));
	} else
	{
		pChn->nNote = static_cast<ModCommand::NOTE>(note);
	}
	pChn->m_CalculateFreq = true;

	if ((!bPorta) || (GetType() & (MOD_TYPE_S3M|MOD_TYPE_IT|MOD_TYPE_MPT)))
		pChn->nNewIns = 0;

	uint32 period = GetPeriodFromNote(note, pChn->nFineTune, pChn->nC5Speed);
	pChn->nPanbrelloOffset = 0;

	// IT compatibility: Sample and instrument panning is only applied on note change, not instrument change
	// Test case: PanReset.it
	if(m_playBehaviour[kITPanningReset]) ApplyInstrumentPanning(pChn, pIns, pSmp);

	if(bResetEnv && !bPorta)
	{
		pChn->nVolSwing = pChn->nPanSwing = 0;
		pChn->nResSwing = pChn->nCutSwing = 0;
		if(pIns)
		{
			// IT Compatiblity: NNA is reset on every note change, not every instrument change (fixes spx-farspacedance.it).
			if(m_playBehaviour[kITNNAReset]) pChn->nNNA = pIns->nNNA;

			if(!pIns->VolEnv.dwFlags[ENV_CARRY]) pChn->VolEnv.Reset();
			if(!pIns->PanEnv.dwFlags[ENV_CARRY]) pChn->PanEnv.Reset();
			if(!pIns->PitchEnv.dwFlags[ENV_CARRY]) pChn->PitchEnv.Reset();

			// Volume Swing
			if(pIns->nVolSwing)
			{
				pChn->nVolSwing = static_cast<int16>(((mpt::random<int8>(AccessPRNG()) * pIns->nVolSwing) / 64 + 1) * (m_playBehaviour[kITSwingBehaviour] ? pChn->nInsVol : ((pChn->nVolume + 1) / 2)) / 199);
			}
			// Pan Swing
			if(pIns->nPanSwing)
			{
				pChn->nPanSwing = static_cast<int16>(((mpt::random<int8>(AccessPRNG()) * pIns->nPanSwing * 4) / 128));
				if(!m_playBehaviour[kITSwingBehaviour])
				{
					pChn->nRestorePanOnNewNote = static_cast<int16>(pChn->nPan + 1);
				}
			}
			// Cutoff Swing
			if(pIns->nCutSwing)
			{
				int32 d = ((int32)pIns->nCutSwing * (int32)(static_cast<int32>(mpt::random<int8>(AccessPRNG())) + 1)) / 128;
				pChn->nCutSwing = static_cast<int16>((d * pChn->nCutOff + 1) / 128);
				pChn->nRestoreCutoffOnNewNote = pChn->nCutOff + 1;
			}
			// Resonance Swing
			if(pIns->nResSwing)
			{
				int32 d = ((int32)pIns->nResSwing * (int32)(static_cast<int32>(mpt::random<int8>(AccessPRNG())) + 1)) / 128;
				pChn->nResSwing = static_cast<int16>((d * pChn->nResonance + 1) / 128);
				pChn->nRestoreResonanceOnNewNote = pChn->nResonance + 1;
			}
		}
	}

	if(!pSmp) return;
	if(period)
	{
		if((!bPorta) || (!pChn->nPeriod)) pChn->nPeriod = period;
		if(!newTuning)
		{
			// FT2 compatibility: Don't reset portamento target with new notes.
			// Test case: Porta-Pickup.xm
			// ProTracker does the same.
			// Test case: PortaTarget.mod
			// IT compatibility: Portamento target is completely cleared with new notes.
			// Test case: PortaReset.it
			if(bPorta || !(m_playBehaviour[kFT2PortaTargetNoReset] || m_playBehaviour[kITClearPortaTarget] || GetType() == MOD_TYPE_MOD))
			{
				pChn->nPortamentoDest = period;
			}
		}

		if(!bPorta || (!pChn->nLength && !(GetType() & MOD_TYPE_S3M)))
		{
			pChn->pModSample = pSmp;
			pChn->nLength = pSmp->nLength;
			pChn->nLoopEnd = pSmp->nLength;
			pChn->nLoopStart = 0;
			pChn->position.Set(0);
			if(m_SongFlags[SONG_PT_MODE] && !pChn->rowCommand.instr)
			{
				pChn->position.SetInt(std::min<SmpLength>(pChn->proTrackerOffset, pChn->nLength - 1));
			} else
			{
				pChn->proTrackerOffset = 0;
			}
			pChn->dwFlags = (pChn->dwFlags & CHN_CHANNELFLAGS) | (pSmp->uFlags & (CHN_SAMPLEFLAGS | CHN_SURROUND));
			pChn->dwFlags.reset(CHN_PORTAMENTO);
			if(pChn->dwFlags[CHN_SUSTAINLOOP])
			{
				pChn->nLoopStart = pSmp->nSustainStart;
				pChn->nLoopEnd = pSmp->nSustainEnd;
				pChn->dwFlags.set(CHN_PINGPONGLOOP, pChn->dwFlags[CHN_PINGPONGSUSTAIN]);
				pChn->dwFlags.set(CHN_LOOP);
				if (pChn->nLength > pChn->nLoopEnd) pChn->nLength = pChn->nLoopEnd;
			} else if(pChn->dwFlags[CHN_LOOP])
			{
				pChn->nLoopStart = pSmp->nLoopStart;
				pChn->nLoopEnd = pSmp->nLoopEnd;
				if (pChn->nLength > pChn->nLoopEnd) pChn->nLength = pChn->nLoopEnd;
			}
			// ProTracker "oneshot" loops (if loop start is 0, play the whole sample once and then repeat until loop end)
			if(m_playBehaviour[kMODOneShotLoops] && pChn->nLoopStart == 0) pChn->nLoopEnd = pChn->nLength = pSmp->nLength;

			if(pChn->dwFlags[CHN_REVERSE])
			{
				pChn->dwFlags.set(CHN_PINGPONGFLAG);
				pChn->position.SetInt(pChn->nLength - 1);
			}

			// Handle "retrigger" waveform type
			if(pChn->nVibratoType < 4)
			{
				// IT Compatibilty: Slightly different waveform offsets (why does MPT have two different offsets here with IT old effects enabled and disabled?)
				if(!m_playBehaviour[kITVibratoTremoloPanbrello] && (GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT)) && !m_SongFlags[SONG_ITOLDEFFECTS])
					pChn->nVibratoPos = 0x10;
				else if(GetType() == MOD_TYPE_MTM)
					pChn->nVibratoPos = 0x20;
				else if(!(GetType() & (MOD_TYPE_DIGI | MOD_TYPE_DBM)))
					pChn->nVibratoPos = 0;
			}
			// IT Compatibility: No "retrigger" waveform here
			if(!m_playBehaviour[kITVibratoTremoloPanbrello] && pChn->nTremoloType < 4)
			{
				pChn->nTremoloPos = 0;
			}
		}
		if(pChn->position.GetUInt() >= pChn->nLength) pChn->position.SetInt(pChn->nLoopStart);
	} else
	{
		bPorta = false;
	}

	if (!bPorta
		|| (!(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_DBM)))
		|| (pChn->dwFlags[CHN_NOTEFADE] && !pChn->nFadeOutVol)
		|| (m_SongFlags[SONG_ITCOMPATGXX] && pChn->rowCommand.instr != 0))
	{
		if((GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_DBM)) && pChn->dwFlags[CHN_NOTEFADE] && !pChn->nFadeOutVol)
		{
			pChn->ResetEnvelopes();
			// IT Compatibility: Autovibrato reset
			if(!m_playBehaviour[kITVibratoTremoloPanbrello])
			{
				pChn->nAutoVibDepth = 0;
				pChn->nAutoVibPos = 0;
			}
			pChn->dwFlags.reset(CHN_NOTEFADE);
			pChn->nFadeOutVol = 65536;
		}
		if ((!bPorta) || (!m_SongFlags[SONG_ITCOMPATGXX]) || (pChn->rowCommand.instr))
		{
			if ((!(GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2))) || (pChn->rowCommand.instr))
			{
				pChn->dwFlags.reset(CHN_NOTEFADE);
				pChn->nFadeOutVol = 65536;
			}
		}
	}

	// IT compatibility: Don't reset key-off flag on porta notes unless Compat Gxx is enabled
	// Test case: Off-Porta.it, Off-Porta-CompatGxx.it
	if(m_playBehaviour[kITDontResetNoteOffOnPorta] && bPorta && (!m_SongFlags[SONG_ITCOMPATGXX] || pChn->rowCommand.instr == 0))
		pChn->dwFlags.reset(CHN_EXTRALOUD);
	else
		pChn->dwFlags.reset(CHN_EXTRALOUD | CHN_KEYOFF);

	// Enable Ramping
	if(!bPorta)
	{
		pChn->nLeftVU = pChn->nRightVU = 0xFF;
		pChn->dwFlags.reset(CHN_FILTER);
		pChn->dwFlags.set(CHN_FASTVOLRAMP);

		// IT compatibility 15. Retrigger is reset in RetrigNote (Tremor doesn't store anything here, so we just don't reset this as well)
		if(!m_playBehaviour[kITRetrigger] && !m_playBehaviour[kITTremor])
		{
			// FT2 compatibility: Retrigger is reset in RetrigNote, tremor in ProcessEffects
			if(!m_playBehaviour[kFT2Retrigger] && !m_playBehaviour[kFT2Tremor])
			{
				pChn->nRetrigCount = 0;
				pChn->nTremorCount = 0;
			}
		}

		if(bResetEnv)
		{
			pChn->nAutoVibDepth = 0;
			pChn->nAutoVibPos = 0;
		}
		pChn->rightVol = pChn->leftVol = 0;
		bool useFilter = !m_SongFlags[SONG_MPTFILTERMODE];
		// Setup Initial Filter for this note
		if(pIns)
		{
			if(pIns->IsResonanceEnabled())
			{
				pChn->nResonance = pIns->GetResonance();
				useFilter = true;
			}
			if(pIns->IsCutoffEnabled())
			{
				pChn->nCutOff = pIns->GetCutoff();
				useFilter = true;
			}
			if(useFilter && (pIns->nFilterMode != FLTMODE_UNCHANGED))
			{
				pChn->nFilterMode = pIns->nFilterMode;
			}
		} else
		{
			pChn->nVolSwing = pChn->nPanSwing = 0;
			pChn->nCutSwing = pChn->nResSwing = 0;
		}
		if((pChn->nCutOff < 0x7F || m_playBehaviour[kITFilterBehaviour]) && useFilter)
		{
			SetupChannelFilter(pChn, true);
		}
	}
	// Special case for MPT
	if (bManual) pChn->dwFlags.reset(CHN_MUTE);
	if((pChn->dwFlags[CHN_MUTE] && (m_MixerSettings.MixerFlags & SNDMIX_MUTECHNMODE))
		|| (pChn->pModSample != nullptr && pChn->pModSample->uFlags[CHN_MUTE] && !bManual)
		|| (pChn->pModInstrument != nullptr && pChn->pModInstrument->dwFlags[INS_MUTE] && !bManual))
	{
		if (!bManual) pChn->nPeriod = 0;
	}

	// Reset the Amiga resampler for this channel
	if(!bPorta)
	{
		pChn->paulaState.Reset();
	}
}