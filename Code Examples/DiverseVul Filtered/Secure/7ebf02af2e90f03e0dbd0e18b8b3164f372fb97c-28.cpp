bool CSoundFile::ProcessEffects()
{
	ModChannel *pChn = m_PlayState.Chn;
	ROWINDEX nBreakRow = ROWINDEX_INVALID;		// Is changed if a break to row command is encountered
	ROWINDEX nPatLoopRow = ROWINDEX_INVALID;	// Is changed if a pattern loop jump-back is executed
	ORDERINDEX nPosJump = ORDERINDEX_INVALID;

	for(CHANNELINDEX nChn = 0; nChn < GetNumChannels(); nChn++, pChn++)
	{
		const uint32 tickCount = m_PlayState.m_nTickCount % (m_PlayState.m_nMusicSpeed + m_PlayState.m_nFrameDelay);
		uint32 instr = pChn->rowCommand.instr;
		ModCommand::VOLCMD volcmd = pChn->rowCommand.volcmd;
		uint32 vol = pChn->rowCommand.vol;
		ModCommand::COMMAND cmd = pChn->rowCommand.command;
		uint32 param = pChn->rowCommand.param;
		bool bPorta = pChn->rowCommand.IsPortamento();

		uint32 nStartTick = 0;
		pChn->isFirstTick = m_SongFlags[SONG_FIRSTTICK];

		// Process parameter control note.
		if(pChn->rowCommand.note == NOTE_PC)
		{
#ifndef NO_PLUGINS
			const PLUGINDEX plug = pChn->rowCommand.instr;
			const PlugParamIndex plugparam = pChn->rowCommand.GetValueVolCol();
			const PlugParamValue value = pChn->rowCommand.GetValueEffectCol() / PlugParamValue(ModCommand::maxColumnValue);

			if(plug > 0 && plug <= MAX_MIXPLUGINS && m_MixPlugins[plug - 1].pMixPlugin)
				m_MixPlugins[plug-1].pMixPlugin->SetParameter(plugparam, value);
#endif // NO_PLUGINS
		}

		// Process continuous parameter control note.
		// Row data is cleared after first tick so on following
		// ticks using channels m_nPlugParamValueStep to identify
		// the need for parameter control. The condition cmd == 0
		// is to make sure that m_nPlugParamValueStep != 0 because
		// of NOTE_PCS, not because of macro.
		if(pChn->rowCommand.note == NOTE_PCS || (cmd == CMD_NONE && pChn->m_plugParamValueStep != 0))
		{
#ifndef NO_PLUGINS
			const bool isFirstTick = m_SongFlags[SONG_FIRSTTICK];
			if(isFirstTick)
				pChn->m_RowPlug = pChn->rowCommand.instr;
			const PLUGINDEX nPlug = pChn->m_RowPlug;
			const bool hasValidPlug = (nPlug > 0 && nPlug <= MAX_MIXPLUGINS && m_MixPlugins[nPlug-1].pMixPlugin);
			if(hasValidPlug)
			{
				if(isFirstTick)
					pChn->m_RowPlugParam = ModCommand::GetValueVolCol(pChn->rowCommand.volcmd, pChn->rowCommand.vol);
				const PlugParamIndex plugparam = pChn->m_RowPlugParam;
				if(isFirstTick)
				{
					PlugParamValue targetvalue = ModCommand::GetValueEffectCol(pChn->rowCommand.command, pChn->rowCommand.param) / PlugParamValue(ModCommand::maxColumnValue);
					pChn->m_plugParamTargetValue = targetvalue;
					pChn->m_plugParamValueStep = (targetvalue - m_MixPlugins[nPlug-1].pMixPlugin->GetParameter(plugparam)) / float(GetNumTicksOnCurrentRow());
				}
				if(m_PlayState.m_nTickCount + 1 == GetNumTicksOnCurrentRow())
				{	// On last tick, set parameter exactly to target value.
					m_MixPlugins[nPlug-1].pMixPlugin->SetParameter(plugparam, pChn->m_plugParamTargetValue);
				}
				else
					m_MixPlugins[nPlug-1].pMixPlugin->ModifyParameter(plugparam, pChn->m_plugParamValueStep);
			}
#endif // NO_PLUGINS
		}

		// Apart from changing parameters, parameter control notes are intended to be 'invisible'.
		// To achieve this, clearing the note data so that rest of the process sees the row as empty row.
		if(ModCommand::IsPcNote(pChn->rowCommand.note))
		{
			pChn->ClearRowCmd();
			instr = 0;
			volcmd = VOLCMD_NONE;
			vol = 0;
			cmd = CMD_NONE;
			param = 0;
			bPorta = false;
		}

		// Process Invert Loop (MOD Effect, called every row if it's active)
		if(!m_SongFlags[SONG_FIRSTTICK])
		{
			InvertLoop(&m_PlayState.Chn[nChn]);
		} else
		{
			if(instr) m_PlayState.Chn[nChn].nEFxOffset = 0;
		}

		// Process special effects (note delay, pattern delay, pattern loop)
		if (cmd == CMD_DELAYCUT)
		{
			//:xy --> note delay until tick x, note cut at tick x+y
			nStartTick = (param & 0xF0) >> 4;
			const uint32 cutAtTick = nStartTick + (param & 0x0F);
			NoteCut(nChn, cutAtTick, m_playBehaviour[kITSCxStopsSample]);
		} else if ((cmd == CMD_MODCMDEX) || (cmd == CMD_S3MCMDEX))
		{
			if ((!param) && (GetType() & (MOD_TYPE_S3M|MOD_TYPE_IT|MOD_TYPE_MPT)))
				param = pChn->nOldCmdEx;
			else
				pChn->nOldCmdEx = static_cast<ModCommand::PARAM>(param);

			// Note Delay ?
			if ((param & 0xF0) == 0xD0)
			{
				nStartTick = param & 0x0F;
				if(nStartTick == 0)
				{
					//IT compatibility 22. SD0 == SD1
					if(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT))
						nStartTick = 1;
					//ST3 ignores notes with SD0 completely
					else if(GetType() == MOD_TYPE_S3M)
						continue;
				} else if(nStartTick >= (m_PlayState.m_nMusicSpeed + m_PlayState.m_nFrameDelay) && m_playBehaviour[kITOutOfRangeDelay])
				{
					// IT compatibility 08. Handling of out-of-range delay command.
					// Additional test case: tickdelay.it
					if(instr)
					{
						pChn->nNewIns = static_cast<ModCommand::INSTR>(instr);
					}
					continue;
				}
			} else if(m_SongFlags[SONG_FIRSTTICK])
			{
				// Pattern Loop ?
				if((((param & 0xF0) == 0x60 && cmd == CMD_MODCMDEX)
					|| ((param & 0xF0) == 0xB0 && cmd == CMD_S3MCMDEX))
					&& !(m_playBehaviour[kST3NoMutedChannels] && ChnSettings[nChn].dwFlags[CHN_MUTE]))	// not even effects are processed on muted S3M channels
				{
					ROWINDEX nloop = PatternLoop(pChn, param & 0x0F);
					if (nloop != ROWINDEX_INVALID)
					{
						// FT2 compatibility: E6x overwrites jump targets of Dxx effects that are located left of the E6x effect.
						// Test cases: PatLoop-Jumps.xm, PatLoop-Various.xm
						if(nBreakRow != ROWINDEX_INVALID && m_playBehaviour[kFT2PatternLoopWithJumps])
						{
							nBreakRow = nloop;
						}

						nPatLoopRow = nloop;
					}

					if(GetType() == MOD_TYPE_S3M)
					{
						// ST3 doesn't have per-channel pattern loop memory, so spam all changes to other channels as well.
						for (CHANNELINDEX i = 0; i < GetNumChannels(); i++)
						{
							m_PlayState.Chn[i].nPatternLoop = pChn->nPatternLoop;
							m_PlayState.Chn[i].nPatternLoopCount = pChn->nPatternLoopCount;
						}
					}
				} else if ((param & 0xF0) == 0xE0)
				{
					// Pattern Delay
					// In Scream Tracker 3 / Impulse Tracker, only the first delay command on this row is considered.
					// Test cases: PatternDelays.it, PatternDelays.s3m, PatternDelays.xm
					// XXX In Scream Tracker 3, the "left" channels are evaluated before the "right" channels, which is not emulated here!
					if(!(GetType() & (MOD_TYPE_S3M | MOD_TYPE_IT | MOD_TYPE_MPT)) || !m_PlayState.m_nPatternDelay)
					{
						if(!(GetType() & (MOD_TYPE_S3M)) || (param & 0x0F) != 0)
						{
							// While Impulse Tracker *does* count S60 as a valid row delay (and thus ignores any other row delay commands on the right),
							// Scream Tracker 3 simply ignores such commands.
							m_PlayState.m_nPatternDelay = 1 + (param & 0x0F);
						}
					}
				}
			}
		}

		if(GetType() == MOD_TYPE_MTM && cmd == CMD_MODCMDEX && (param & 0xF0) == 0xD0)
		{
			// Apparently, retrigger and note delay have the same behaviour in MultiTracker:
			// They both restart the note at tick x, and if there is a note on the same row,
			// this note is started on the first tick.
			nStartTick = 0;
			param = 0x90 | (param & 0x0F);
		}

		if(nStartTick != 0 && pChn->rowCommand.note == NOTE_KEYOFF && pChn->rowCommand.volcmd == VOLCMD_PANNING && m_playBehaviour[kFT2PanWithDelayedNoteOff])
		{
			// FT2 compatibility: If there's a delayed note off, panning commands are ignored. WTF!
			// Test case: PanOff.xm
			pChn->rowCommand.volcmd = VOLCMD_NONE;
		}

		bool triggerNote = (m_PlayState.m_nTickCount == nStartTick);	// Can be delayed by a note delay effect
		if(m_playBehaviour[kFT2OutOfRangeDelay] && nStartTick >= m_PlayState.m_nMusicSpeed)
		{
			// FT2 compatibility: Note delays greater than the song speed should be ignored.
			// However, EEx pattern delay is *not* considered at all.
			// Test case: DelayCombination.xm, PortaDelay.xm
			triggerNote = false;
		} else if(m_playBehaviour[kRowDelayWithNoteDelay] && nStartTick > 0 && tickCount == nStartTick)
		{
			// IT compatibility: Delayed notes (using SDx) that are on the same row as a Row Delay effect are retriggered.
			// ProTracker / Scream Tracker 3 / FastTracker 2 do the same.
			// Test case: PatternDelay-NoteDelay.it, PatternDelay-NoteDelay.xm, PatternDelaysRetrig.mod
			triggerNote = true;
		}

		// IT compatibility: Tick-0 vs non-tick-0 effect distinction is always based on tick delay.
		// Test case: SlideDelay.it
		if(m_playBehaviour[kITFirstTickHandling])
		{
			pChn->isFirstTick = tickCount == nStartTick;
		}

		// FT2 compatibility: Note + portamento + note delay = no portamento
		// Test case: PortaDelay.xm
		if(m_playBehaviour[kFT2PortaDelay] && nStartTick != 0)
		{
			bPorta = false;
		}

		if(m_SongFlags[SONG_PT_MODE] && instr && !m_PlayState.m_nTickCount)
		{
			// Instrument number resets the stacked ProTracker offset.
			// Test case: ptoffset.mod
			pChn->proTrackerOffset = 0;
			// ProTracker compatibility: Sample properties are always loaded on the first tick, even when there is a note delay.
			// Test case: InstrDelay.mod
			if(!triggerNote && pChn->IsSamplePlaying())
			{
				pChn->nNewIns = static_cast<ModCommand::INSTR>(instr);
				if(instr <= GetNumSamples())
				{
					pChn->nVolume = Samples[instr].nVolume;
					pChn->nFineTune = Samples[instr].nFineTune;
				}
			}
		}

		// Handles note/instrument/volume changes
		if(triggerNote)
		{
			ModCommand::NOTE note = pChn->rowCommand.note;
			if(instr) pChn->nNewIns = static_cast<ModCommand::INSTR>(instr);

			if(ModCommand::IsNote(note) && m_playBehaviour[kFT2Transpose])
			{
				// Notes that exceed FT2's limit are completely ignored.
				// Test case: NoteLimit.xm
				int transpose = pChn->nTranspose;
				if(instr && !bPorta)
				{
					// Refresh transpose
					// Test case: NoteLimit2.xm
					SAMPLEINDEX sample = SAMPLEINDEX_INVALID;
					if(GetNumInstruments())
					{
						// Instrument mode
						if(instr <= GetNumInstruments() && Instruments[instr] != nullptr)
						{
							sample = Instruments[instr]->Keyboard[note - NOTE_MIN];
						}
					} else
					{
						// Sample mode
						sample = static_cast<SAMPLEINDEX>(instr);
					}
					if(sample <= GetNumSamples())
					{
						transpose = GetSample(sample).RelativeTone;
					}
				}

				const int computedNote = note + transpose;
				if((computedNote < NOTE_MIN + 11 || computedNote > NOTE_MIN + 130))
				{
					note = NOTE_NONE;
				}
			} else if((GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT)) && GetNumInstruments() != 0 && ModCommand::IsNoteOrEmpty(static_cast<ModCommand::NOTE>(note)))
			{
				// IT compatibility: Invalid instrument numbers do nothing, but they are remembered for upcoming notes and do not trigger a note in that case.
				// Test case: InstrumentNumberChange.it
				INSTRUMENTINDEX instrToCheck = static_cast<INSTRUMENTINDEX>((instr != 0) ? instr : pChn->nOldIns);
				if(instrToCheck != 0 && (instrToCheck > GetNumInstruments() || Instruments[instrToCheck] == nullptr))
				{
					note = NOTE_NONE;
					instr = 0;
				}
			}

			// XM: FT2 ignores a note next to a K00 effect, and a fade-out seems to be done when no volume envelope is present (not exactly the Kxx behaviour)
			if(cmd == CMD_KEYOFF && param == 0 && m_playBehaviour[kFT2KeyOff])
			{
				note = NOTE_NONE;
				instr = 0;
			}

			bool retrigEnv = note == NOTE_NONE && instr != 0;

			// Apparently, any note number in a pattern causes instruments to recall their original volume settings - no matter if there's a Note Off next to it or whatever.
			// Test cases: keyoff+instr.xm, delay.xm
			bool reloadSampleSettings = (m_playBehaviour[kFT2ReloadSampleSettings] && instr != 0);
			// ProTracker Compatibility: If a sample was stopped before, lone instrument numbers can retrigger it
			// Test case: PTSwapEmpty.mod
			bool keepInstr = (GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT)) || (m_playBehaviour[kMODSampleSwap] && !pChn->IsSamplePlaying() && pChn->pModSample != nullptr && !pChn->pModSample->HasSampleData());

			// Now it's time for some FT2 crap...
			if (GetType() & (MOD_TYPE_XM | MOD_TYPE_MT2))
			{

				// XM: Key-Off + Sample == Note Cut (BUT: Only if no instr number or volume effect is present!)
				// Test case: NoteOffVolume.xm
				if(note == NOTE_KEYOFF
					&& ((!instr && volcmd != VOLCMD_VOLUME && cmd != CMD_VOLUME) || !m_playBehaviour[kFT2KeyOff])
					&& (pChn->pModInstrument == nullptr || !pChn->pModInstrument->VolEnv.dwFlags[ENV_ENABLED]))
				{
					pChn->dwFlags.set(CHN_FASTVOLRAMP);
					pChn->nVolume = 0;
					note = NOTE_NONE;
					instr = 0;
					retrigEnv = false;
					// FT2 Compatbility: Start fading the note for notes with no delay. Only relevant when a volume command is encountered after the note-off.
					// Test case: NoteOffFadeNoEnv.xm
					if(m_SongFlags[SONG_FIRSTTICK] && m_playBehaviour[kFT2NoteOffFlags])
						pChn->dwFlags.set(CHN_NOTEFADE);
				} else if(m_playBehaviour[kFT2RetrigWithNoteDelay] && !m_SongFlags[SONG_FIRSTTICK])
				{
					// FT2 Compatibility: Some special hacks for rogue note delays... (EDx with x > 0)
					// Apparently anything that is next to a note delay behaves totally unpredictable in FT2. Swedish tracker logic. :)

					retrigEnv = true;

					// Portamento + Note Delay = No Portamento
					// Test case: porta-delay.xm
					bPorta = false;

					if(note == NOTE_NONE)
					{
						// If there's a note delay but no real note, retrig the last note.
						// Test case: delay2.xm, delay3.xm
						note = static_cast<ModCommand::NOTE>(pChn->nNote - pChn->nTranspose);
					} else if(note >= NOTE_MIN_SPECIAL)
					{
						// Gah! Even Note Off + Note Delay will cause envelopes to *retrigger*! How stupid is that?
						// ... Well, and that is actually all it does if there's an envelope. No fade out, no nothing. *sigh*
						// Test case: OffDelay.xm
						note = NOTE_NONE;
						keepInstr = false;
						reloadSampleSettings = true;
					} else
					{
						// Normal note
						keepInstr = true;
						reloadSampleSettings = true;
					}
				}
			}

			if((retrigEnv && !m_playBehaviour[kFT2ReloadSampleSettings]) || reloadSampleSettings)
			{
				const ModSample *oldSample = nullptr;
				// Reset default volume when retriggering envelopes

				if(GetNumInstruments())
				{
					oldSample = pChn->pModSample;
				} else if (instr <= GetNumSamples())
				{
					// Case: Only samples are used; no instruments.
					oldSample = &Samples[instr];
				}

				if(oldSample != nullptr)
				{
					if(!oldSample->uFlags[SMP_NODEFAULTVOLUME])
						pChn->nVolume = oldSample->nVolume;
					if(reloadSampleSettings)
					{
						// Also reload panning
						pChn->nPan = oldSample->nPan;
					}
				}
			}

			// FT2 compatibility: Instrument number disables tremor effect
			// Test case: TremorInstr.xm, TremoRecover.xm
			if(m_playBehaviour[kFT2Tremor] && instr != 0)
			{
				pChn->nTremorCount = 0x20;
			}

			if(retrigEnv) //Case: instrument with no note data.
			{
				//IT compatibility: Instrument with no note.
				if(m_playBehaviour[kITInstrWithoutNote] || GetType() == MOD_TYPE_PLM)
				{
					// IT compatibility: Completely retrigger note after sample end to also reset portamento.
					// Test case: PortaResetAfterRetrigger.it
					bool triggerAfterSmpEnd = m_playBehaviour[kITMultiSampleInstrumentNumber] && !pChn->IsSamplePlaying();
					if(GetNumInstruments())
					{
						// Instrument mode
						if(instr <= GetNumInstruments() && (pChn->pModInstrument != Instruments[instr] || triggerAfterSmpEnd))
							note = pChn->nNote;
					} else
					{
						// Sample mode
						if(instr < MAX_SAMPLES && (pChn->pModSample != &Samples[instr] || triggerAfterSmpEnd))
							note = pChn->nNote;
					}
				}

				if (GetNumInstruments() && (GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2)))
				{
					pChn->ResetEnvelopes();
					pChn->dwFlags.set(CHN_FASTVOLRAMP);
					pChn->dwFlags.reset(CHN_NOTEFADE);
					pChn->nAutoVibDepth = 0;
					pChn->nAutoVibPos = 0;
					pChn->nFadeOutVol = 65536;
					// FT2 Compatbility: Reset key-off status with instrument number
					// Test case: NoteOffInstrChange.xm
					if(m_playBehaviour[kFT2NoteOffFlags])
						pChn->dwFlags.reset(CHN_KEYOFF);
				}
				if (!keepInstr) instr = 0;
			}

			// Note Cut/Off/Fade => ignore instrument
			if (note >= NOTE_MIN_SPECIAL)
			{
				// IT compatibility: Default volume of sample is recalled if instrument number is next to a note-off.
				// Test case: NoteOffInstr.it, noteoff2.it
				if(m_playBehaviour[kITInstrWithNoteOff] && instr)
				{
					SAMPLEINDEX smp = static_cast<SAMPLEINDEX>(instr);
					if(GetNumInstruments())
					{
						smp = 0;
						if(instr <= GetNumInstruments() && Instruments[instr] != nullptr && ModCommand::IsNote(pChn->nLastNote))
						{
							smp = Instruments[instr]->Keyboard[pChn->nLastNote - NOTE_MIN];
						}
					}
					if(smp > 0 && smp <= GetNumSamples() && !Samples[smp].uFlags[SMP_NODEFAULTVOLUME])
						pChn->nVolume = Samples[smp].nVolume;
				}
				instr = 0;
			}

			if(ModCommand::IsNote(note))
			{
				pChn->nNewNote = pChn->nLastNote = note;

				// New Note Action ?
				if(!bPorta)
				{
					CheckNNA(nChn, instr, note, false);
				}
			}

			if(note)
			{
				if(pChn->nRestorePanOnNewNote > 0)
				{
					pChn->nPan = pChn->nRestorePanOnNewNote - 1;
					pChn->nRestorePanOnNewNote = 0;
				}
				if(pChn->nRestoreResonanceOnNewNote > 0)
				{
					pChn->nResonance = pChn->nRestoreResonanceOnNewNote - 1;
					pChn->nRestoreResonanceOnNewNote = 0;
				}
				if(pChn->nRestoreCutoffOnNewNote > 0)
				{
					pChn->nCutOff = pChn->nRestoreCutoffOnNewNote - 1;
					pChn->nRestoreCutoffOnNewNote = 0;
				}
			}

			// Instrument Change ?
			if(instr)
			{
				const ModSample *oldSample = pChn->pModSample;
				//const ModInstrument *oldInstrument = pChn->pModInstrument;

				InstrumentChange(pChn, instr, bPorta, true);
				// IT compatibility: Keep new instrument number for next instrument-less note even if sample playback is stopped
				// Test case: StoppedInstrSwap.it
				if(GetType() == MOD_TYPE_MOD)
				{
					// Test case: PortaSwapPT.mod
					if(!bPorta || !m_playBehaviour[kMODSampleSwap]) pChn->nNewIns = 0;
				} else
				{
					if(!m_playBehaviour[kITInstrWithNoteOff] || ModCommand::IsNote(note)) pChn->nNewIns = 0;
				}

				if(m_playBehaviour[kITPortamentoSwapResetsPos])
				{
					// Test cases: PortaInsNum.it, PortaSample.it
					if(ModCommand::IsNote(note) && oldSample != pChn->pModSample)
					{
						//const bool newInstrument = oldInstrument != pChn->pModInstrument && pChn->pModInstrument->Keyboard[pChn->nNewNote - NOTE_MIN] != 0;
						pChn->position.Set(0);
					}
				} else if ((GetType() & (MOD_TYPE_S3M | MOD_TYPE_IT | MOD_TYPE_MPT) && oldSample != pChn->pModSample && ModCommand::IsNote(note)))
				{
					// Special IT case: portamento+note causes sample change -> ignore portamento
					bPorta = false;
				} else if(m_playBehaviour[kMODSampleSwap] && pChn->increment.IsZero())
				{
					// If channel was paused and is resurrected by a lone instrument number, reset the sample position.
					// Test case: PTSwapEmpty.mod
					pChn->position.Set(0);
				}
			}
			// New Note ?
			if (note)
			{
				if ((!instr) && (pChn->nNewIns) && (note < 0x80))
				{
					InstrumentChange(pChn, pChn->nNewIns, bPorta, pChn->pModSample == nullptr && pChn->pModInstrument == nullptr, !(GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2)));
					pChn->nNewIns = 0;
				}
				NoteChange(pChn, note, bPorta, !(GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2)));
				if ((bPorta) && (GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2)) && (instr))
				{
					pChn->dwFlags.set(CHN_FASTVOLRAMP);
					pChn->ResetEnvelopes();
					pChn->nAutoVibDepth = 0;
					pChn->nAutoVibPos = 0;
				}
			}
			// Tick-0 only volume commands
			if (volcmd == VOLCMD_VOLUME)
			{
				if (vol > 64) vol = 64;
				pChn->nVolume = vol << 2;
				pChn->dwFlags.set(CHN_FASTVOLRAMP);
			} else
			if (volcmd == VOLCMD_PANNING)
			{
				Panning(pChn, vol, Pan6bit);
			}

#ifndef NO_PLUGINS
			if (m_nInstruments) ProcessMidiOut(nChn);
#endif // NO_PLUGINS
		}

		if(m_playBehaviour[kST3NoMutedChannels] && ChnSettings[nChn].dwFlags[CHN_MUTE])	// not even effects are processed on muted S3M channels
			continue;

		// Volume Column Effect (except volume & panning)
		/*	A few notes, paraphrased from ITTECH.TXT by Storlek (creator of schismtracker):
			Ex/Fx/Gx are shared with Exx/Fxx/Gxx; Ex/Fx are 4x the 'normal' slide value
			Gx is linked with Ex/Fx if Compat Gxx is off, just like Gxx is with Exx/Fxx
			Gx values: 1, 4, 8, 16, 32, 64, 96, 128, 255
			Ax/Bx/Cx/Dx values are used directly (i.e. D9 == D09), and are NOT shared with Dxx
			(value is stored into nOldVolParam and used by A0/B0/C0/D0)
			Hx uses the same value as Hxx and Uxx, and affects the *depth*
			so... hxx = (hx | (oldhxx & 0xf0))  ???
			TODO is this done correctly?
		*/
		bool doVolumeColumn = m_PlayState.m_nTickCount >= nStartTick;
		// FT2 compatibility: If there's a note delay, volume column effects are NOT executed
		// on the first tick and, if there's an instrument number, on the delayed tick.
		// Test case: VolColDelay.xm, PortaDelay.xm
		if(m_playBehaviour[kFT2VolColDelay] && nStartTick != 0)
		{
			doVolumeColumn = m_PlayState.m_nTickCount != 0 && (m_PlayState.m_nTickCount != nStartTick || (pChn->rowCommand.instr == 0 && volcmd != VOLCMD_TONEPORTAMENTO));
		}
		if(volcmd > VOLCMD_PANNING && doVolumeColumn)
		{
			if (volcmd == VOLCMD_TONEPORTAMENTO)
			{
				uint32 porta = 0;
				if(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_AMS | MOD_TYPE_AMS2 | MOD_TYPE_DMF | MOD_TYPE_DBM | MOD_TYPE_IMF | MOD_TYPE_PSM | MOD_TYPE_J2B | MOD_TYPE_ULT | MOD_TYPE_OKT | MOD_TYPE_MT2 | MOD_TYPE_MDL))
				{
					porta = ImpulseTrackerPortaVolCmd[vol & 0x0F];
				} else
				{
					if(cmd == CMD_TONEPORTAMENTO && GetType() == MOD_TYPE_XM)
					{
						// Yes, FT2 is *that* weird. If there is a Mx command in the volume column
						// and a normal 3xx command, the 3xx command is ignored but the Mx command's
						// effectiveness is doubled.
						// Test case: TonePortamentoMemory.xm
						cmd = CMD_NONE;
						vol *= 2;
					}
					porta = vol << 4;

					// FT2 compatibility: If there's a portamento and a note delay, execute the portamento, but don't update the parameter
					// Test case: PortaDelay.xm
					if(m_playBehaviour[kFT2PortaDelay] && nStartTick != 0)
					{
						porta = 0;
					}
				}
				TonePortamento(pChn, porta);
			} else
			{
				// FT2 Compatibility: FT2 ignores some volume commands with parameter = 0.
				if(m_playBehaviour[kFT2VolColMemory] && vol == 0)
				{
					switch(volcmd)
					{
					case VOLCMD_VOLUME:
					case VOLCMD_PANNING:
					case VOLCMD_VIBRATODEPTH:
						break;
					case VOLCMD_PANSLIDELEFT:
						// FT2 Compatibility: Pan slide left with zero parameter causes panning to be set to full left on every non-row tick.
						// Test case: PanSlideZero.xm
						if(!m_SongFlags[SONG_FIRSTTICK])
						{
							pChn->nPan = 0;
						}
						MPT_FALLTHROUGH;
					default:
						// no memory here.
						volcmd = VOLCMD_NONE;
					}

				} else if(!m_playBehaviour[kITVolColMemory])
				{
					// IT Compatibility: Effects in the volume column don't have an unified memory.
					// Test case: VolColMemory.it
					if(vol) pChn->nOldVolParam = static_cast<ModCommand::PARAM>(vol); else vol = pChn->nOldVolParam;
				}

				switch(volcmd)
				{
				case VOLCMD_VOLSLIDEUP:
				case VOLCMD_VOLSLIDEDOWN:
					// IT Compatibility: Volume column volume slides have their own memory
					// Test case: VolColMemory.it
					if(vol == 0 && m_playBehaviour[kITVolColMemory])
					{
						vol = pChn->nOldVolParam;
						if(vol == 0)
							break;
					} else
					{
						pChn->nOldVolParam = static_cast<ModCommand::PARAM>(vol);
					}
					VolumeSlide(pChn, static_cast<ModCommand::PARAM>(volcmd == VOLCMD_VOLSLIDEUP ? (vol << 4) : vol));
					break;

				case VOLCMD_FINEVOLUP:
					// IT Compatibility: Fine volume slides in the volume column are only executed on the first tick, not on multiples of the first tick in case of pattern delay
					// Test case: FineVolColSlide.it
					if(m_PlayState.m_nTickCount == nStartTick || !m_playBehaviour[kITVolColMemory])
					{
						// IT Compatibility: Volume column volume slides have their own memory
						// Test case: VolColMemory.it
						FineVolumeUp(pChn, static_cast<ModCommand::PARAM>(vol), m_playBehaviour[kITVolColMemory]);
					}
					break;

				case VOLCMD_FINEVOLDOWN:
					// IT Compatibility: Fine volume slides in the volume column are only executed on the first tick, not on multiples of the first tick in case of pattern delay
					// Test case: FineVolColSlide.it
					if(m_PlayState.m_nTickCount == nStartTick || !m_playBehaviour[kITVolColMemory])
					{
						// IT Compatibility: Volume column volume slides have their own memory
						// Test case: VolColMemory.it
						FineVolumeDown(pChn, static_cast<ModCommand::PARAM>(vol), m_playBehaviour[kITVolColMemory]);
					}
					break;

				case VOLCMD_VIBRATOSPEED:
					// FT2 does not automatically enable vibrato with the "set vibrato speed" command
					if(m_playBehaviour[kFT2VolColVibrato])
						pChn->nVibratoSpeed = vol & 0x0F;
					else
						Vibrato(pChn, vol << 4);
					break;

				case VOLCMD_VIBRATODEPTH:
					Vibrato(pChn, vol);
					break;

				case VOLCMD_PANSLIDELEFT:
					PanningSlide(pChn, static_cast<ModCommand::PARAM>(vol), !m_playBehaviour[kFT2VolColMemory]);
					break;

				case VOLCMD_PANSLIDERIGHT:
					PanningSlide(pChn, static_cast<ModCommand::PARAM>(vol << 4), !m_playBehaviour[kFT2VolColMemory]);
					break;

				case VOLCMD_PORTAUP:
					// IT compatibility (one of the first testcases - link effect memory)
					PortamentoUp(nChn, static_cast<ModCommand::PARAM>(vol << 2), m_playBehaviour[kITVolColFinePortamento]);
					break;

				case VOLCMD_PORTADOWN:
					// IT compatibility (one of the first testcases - link effect memory)
					PortamentoDown(nChn, static_cast<ModCommand::PARAM>(vol << 2), m_playBehaviour[kITVolColFinePortamento]);
					break;

				case VOLCMD_OFFSET:
					if (triggerNote && pChn->pModSample && vol <= CountOf(pChn->pModSample->cues))
					{
						SmpLength offset;
						if(vol == 0)
							offset = pChn->oldOffset;
						else
							offset = pChn->oldOffset = pChn->pModSample->cues[vol - 1];
						SampleOffset(*pChn, offset);
					}
					break;
				}
			}
		}

		// Effects
		if(cmd != CMD_NONE) switch (cmd)
		{
		// Set Volume
		case CMD_VOLUME:
			if(m_SongFlags[SONG_FIRSTTICK])
			{
				pChn->nVolume = (param < 64) ? param * 4 : 256;
				pChn->dwFlags.set(CHN_FASTVOLRAMP);
			}
			break;

		// Portamento Up
		case CMD_PORTAMENTOUP:
			if ((!param) && (GetType() & MOD_TYPE_MOD)) break;
			PortamentoUp(nChn, static_cast<ModCommand::PARAM>(param));
			break;

		// Portamento Down
		case CMD_PORTAMENTODOWN:
			if ((!param) && (GetType() & MOD_TYPE_MOD)) break;
			PortamentoDown(nChn, static_cast<ModCommand::PARAM>(param));
			break;

		// Volume Slide
		case CMD_VOLUMESLIDE:
			if (param || (GetType() != MOD_TYPE_MOD)) VolumeSlide(pChn, static_cast<ModCommand::PARAM>(param));
			break;

		// Tone-Portamento
		case CMD_TONEPORTAMENTO:
			TonePortamento(pChn, param);
			break;

		// Tone-Portamento + Volume Slide
		case CMD_TONEPORTAVOL:
			if ((param) || (GetType() != MOD_TYPE_MOD)) VolumeSlide(pChn, static_cast<ModCommand::PARAM>(param));
			TonePortamento(pChn, 0);
			break;

		// Vibrato
		case CMD_VIBRATO:
			Vibrato(pChn, param);
			break;

		// Vibrato + Volume Slide
		case CMD_VIBRATOVOL:
			if ((param) || (GetType() != MOD_TYPE_MOD)) VolumeSlide(pChn, static_cast<ModCommand::PARAM>(param));
			Vibrato(pChn, 0);
			break;

		// Set Speed
		case CMD_SPEED:
			if(m_SongFlags[SONG_FIRSTTICK])
				SetSpeed(m_PlayState, param);
			break;

		// Set Tempo
		case CMD_TEMPO:
			if(m_playBehaviour[kMODVBlankTiming])
			{
				// ProTracker MODs with VBlank timing: All Fxx parameters set the tick count.
				if(m_SongFlags[SONG_FIRSTTICK] && param != 0) SetSpeed(m_PlayState, param);
				break;
			}
			{
				param = CalculateXParam(m_PlayState.m_nPattern, m_PlayState.m_nRow, nChn);
				if (GetType() & (MOD_TYPE_S3M|MOD_TYPE_IT|MOD_TYPE_MPT))
				{
					if (param) pChn->nOldTempo = static_cast<ModCommand::PARAM>(param); else param = pChn->nOldTempo;
				}
				TEMPO t(param, 0);
				LimitMax(t, GetModSpecifications().GetTempoMax());
				SetTempo(t);
			}
			break;

		// Set Offset
		case CMD_OFFSET:
			if (triggerNote)
			{
				// FT2 compatibility: Portamento + Offset = Ignore offset
				// Test case: porta-offset.xm
				if(bPorta && GetType() == MOD_TYPE_XM)
				{
					break;
				}
				bool isExtended = false;
				SmpLength offset = CalculateXParam(m_PlayState.m_nPattern, m_PlayState.m_nRow, nChn, &isExtended);
				if(!isExtended)
				{
					// No X-param (normal behaviour)
					offset <<= 8;
					if (offset) pChn->oldOffset = offset; else offset = pChn->oldOffset;
					offset += static_cast<SmpLength>(pChn->nOldHiOffset) << 16;
				}
				SampleOffset(*pChn, offset);
			}
			break;

		// Disorder Tracker 2 percentage offset
		case CMD_OFFSETPERCENTAGE:
			if(triggerNote)
			{
				SampleOffset(*pChn, Util::muldiv_unsigned(pChn->nLength, param, 255));
			}
			break;

		// Arpeggio
		case CMD_ARPEGGIO:
			// IT compatibility 01. Don't ignore Arpeggio if no note is playing (also valid for ST3)
			if(m_PlayState.m_nTickCount) break;
			if((!pChn->nPeriod || !pChn->nNote)
				&& (pChn->pModInstrument == nullptr || !pChn->pModInstrument->HasValidMIDIChannel())	// Plugin arpeggio
				&& !m_playBehaviour[kITArpeggio] && (GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT))) break;
			if (!param && (GetType() & (MOD_TYPE_XM | MOD_TYPE_MOD))) break;	// Only important when editing MOD/XM files (000 effects are removed when loading files where this means "no effect")
			pChn->nCommand = CMD_ARPEGGIO;
			if (param) pChn->nArpeggio = static_cast<ModCommand::PARAM>(param);
			break;

		// Retrig
		case CMD_RETRIG:
			if (GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2))
			{
				if (!(param & 0xF0)) param |= pChn->nRetrigParam & 0xF0;
				if (!(param & 0x0F)) param |= pChn->nRetrigParam & 0x0F;
				param |= 0x100; // increment retrig count on first row
			}
			// IT compatibility 15. Retrigger
			if(m_playBehaviour[kITRetrigger])
			{
				if (param) pChn->nRetrigParam = static_cast<uint8>(param & 0xFF);
				RetrigNote(nChn, pChn->nRetrigParam, (volcmd == VOLCMD_OFFSET) ? vol + 1 : 0);
			} else
			{
				// XM Retrig
				if (param) pChn->nRetrigParam = static_cast<uint8>(param & 0xFF); else param = pChn->nRetrigParam;
				RetrigNote(nChn, param, (volcmd == VOLCMD_OFFSET) ? vol + 1 : 0);
			}
			break;

		// Tremor
		case CMD_TREMOR:
			if(!m_SongFlags[SONG_FIRSTTICK])
			{
				break;
			}

			// IT compatibility 12. / 13. Tremor (using modified DUMB's Tremor logic here because of old effects - http://dumb.sf.net/)
			if(m_playBehaviour[kITTremor])
			{
				if(param && !m_SongFlags[SONG_ITOLDEFFECTS])
				{
					// Old effects have different length interpretation (+1 for both on and off)
					if(param & 0xF0) param -= 0x10;
					if(param & 0x0F) param -= 0x01;
				}
				pChn->nTremorCount |= 0x80; // set on/off flag
			} else if(m_playBehaviour[kFT2Tremor])
			{
				// XM Tremor. Logic is being processed in sndmix.cpp
				pChn->nTremorCount |= 0x80; // set on/off flag
			}

			pChn->nCommand = CMD_TREMOR;
			if (param) pChn->nTremorParam = static_cast<ModCommand::PARAM>(param);

			break;

		// Set Global Volume
		case CMD_GLOBALVOLUME:
			// IT compatibility: Only apply global volume on first tick (and multiples)
			// Test case: GlobalVolFirstTick.it
			if(!m_SongFlags[SONG_FIRSTTICK])
				break;
			// ST3 applies global volume on tick 1 and does other weird things, but we won't emulate this for now.
// 			if(((GetType() & MOD_TYPE_S3M) && m_nTickCount != 1)
// 				|| (!(GetType() & MOD_TYPE_S3M) && !m_SongFlags[SONG_FIRSTTICK]))
// 			{
// 				break;
// 			}

			// FT2 compatibility: On channels that are "left" of the global volume command, the new global volume is not applied
			// until the second tick of the row. Since we apply global volume on the mix buffer rather than note volumes, this
			// cannot be fixed for now.
			// Test case: GlobalVolume.xm
// 			if(IsCompatibleMode(TRK_FASTTRACKER2) && m_SongFlags[SONG_FIRSTTICK] && m_nMusicSpeed > 1)
// 			{
// 				break;
// 			}

			if (!(GetType() & GLOBALVOL_7BIT_FORMATS)) param *= 2;

			// IT compatibility 16. ST3 and IT ignore out-of-range values.
			// Test case: globalvol-invalid.it
			if(param <= 128)
			{
				m_PlayState.m_nGlobalVolume = param * 2;
			} else if(!(GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_S3M)))
			{
				m_PlayState.m_nGlobalVolume = 256;
			}
			break;

		// Global Volume Slide
		case CMD_GLOBALVOLSLIDE:
			//IT compatibility 16. Saving last global volume slide param per channel (FT2/IT)
			if(m_playBehaviour[kPerChannelGlobalVolSlide])
				GlobalVolSlide(static_cast<ModCommand::PARAM>(param), pChn->nOldGlobalVolSlide);
			else
				GlobalVolSlide(static_cast<ModCommand::PARAM>(param), m_PlayState.Chn[0].nOldGlobalVolSlide);
			break;

		// Set 8-bit Panning
		case CMD_PANNING8:
			if(m_SongFlags[SONG_FIRSTTICK])
			{
				Panning(pChn, param, Pan8bit);
			}
			break;

		// Panning Slide
		case CMD_PANNINGSLIDE:
			PanningSlide(pChn, static_cast<ModCommand::PARAM>(param));
			break;

		// Tremolo
		case CMD_TREMOLO:
			Tremolo(pChn, param);
			break;

		// Fine Vibrato
		case CMD_FINEVIBRATO:
			FineVibrato(pChn, param);
			break;

		// MOD/XM Exx Extended Commands
		case CMD_MODCMDEX:
			ExtendedMODCommands(nChn, static_cast<ModCommand::PARAM>(param));
			break;

		// S3M/IT Sxx Extended Commands
		case CMD_S3MCMDEX:
			if(m_playBehaviour[kST3EffectMemory] && param == 0)
			{
				param = pChn->nArpeggio;	// S00 uses the last non-zero effect parameter as memory, like other effects including Arpeggio, so we "borrow" our memory there.
			}
			ExtendedS3MCommands(nChn, static_cast<ModCommand::PARAM>(param));
			break;

		// Key Off
		case CMD_KEYOFF:
			// This is how Key Off is supposed to sound... (in FT2 at least)
			if(m_playBehaviour[kFT2KeyOff])
			{
				if (m_PlayState.m_nTickCount == param)
				{
					// XM: Key-Off + Sample == Note Cut
					if(pChn->pModInstrument == nullptr || !pChn->pModInstrument->VolEnv.dwFlags[ENV_ENABLED])
					{
						if(param == 0 && (pChn->rowCommand.instr || pChn->rowCommand.volcmd != VOLCMD_NONE)) // FT2 is weird....
						{
							pChn->dwFlags.set(CHN_NOTEFADE);
						}
						else
						{
							pChn->dwFlags.set(CHN_FASTVOLRAMP);
							pChn->nVolume = 0;
						}
					}
					KeyOff(pChn);
				}
			}
			// This is how it's NOT supposed to sound...
			else
			{
				if(m_SongFlags[SONG_FIRSTTICK])
					KeyOff(pChn);
			}
			break;

		// Extra-fine porta up/down
		case CMD_XFINEPORTAUPDOWN:
			switch(param & 0xF0)
			{
			case 0x10: ExtraFinePortamentoUp(pChn, param & 0x0F); break;
			case 0x20: ExtraFinePortamentoDown(pChn, param & 0x0F); break;
			// ModPlug XM Extensions (ignore in compatible mode)
			case 0x50:
			case 0x60:
			case 0x70:
			case 0x90:
			case 0xA0:
				if(!m_playBehaviour[kFT2RestrictXCommand]) ExtendedS3MCommands(nChn, static_cast<ModCommand::PARAM>(param));
				break;
			}
			break;

		// Set Channel Global Volume
		case CMD_CHANNELVOLUME:
			if(!m_SongFlags[SONG_FIRSTTICK]) break;
			if (param <= 64)
			{
				pChn->nGlobalVol = param;
				pChn->dwFlags.set(CHN_FASTVOLRAMP);
			}
			break;

		// Channel volume slide
		case CMD_CHANNELVOLSLIDE:
			ChannelVolSlide(pChn, static_cast<ModCommand::PARAM>(param));
			break;

		// Panbrello (IT)
		case CMD_PANBRELLO:
			Panbrello(pChn, param);
			break;

		// Set Envelope Position
		case CMD_SETENVPOSITION:
			if(m_SongFlags[SONG_FIRSTTICK])
			{
				pChn->VolEnv.nEnvPosition = param;

				// FT2 compatibility: FT2 only sets the position of the panning envelope if the volume envelope's sustain flag is set
				// Test case: SetEnvPos.xm
				if(!m_playBehaviour[kFT2SetPanEnvPos] || pChn->VolEnv.flags[ENV_SUSTAIN])
				{
					pChn->PanEnv.nEnvPosition = param;
					pChn->PitchEnv.nEnvPosition = param;
				}

			}
			break;

		// Position Jump
		case CMD_POSITIONJUMP:
			m_PlayState.m_nNextPatStartRow = 0; // FT2 E60 bug
			nPosJump = static_cast<ORDERINDEX>(CalculateXParam(m_PlayState.m_nPattern, m_PlayState.m_nRow, nChn));

			// see https://forum.openmpt.org/index.php?topic=2769.0 - FastTracker resets Dxx if Bxx is called _after_ Dxx
			// Test case: PatternJump.mod
			if((GetType() & (MOD_TYPE_MOD | MOD_TYPE_XM)) && nBreakRow != ROWINDEX_INVALID)
			{
				nBreakRow = 0;
			}
			break;

		// Pattern Break
		case CMD_PATTERNBREAK:
			{
				ROWINDEX row = PatternBreak(m_PlayState, nChn, static_cast<ModCommand::PARAM>(param));
				if(row != ROWINDEX_INVALID)
				{
					nBreakRow = row;
					if(m_SongFlags[SONG_PATTERNLOOP])
					{
						//If song is set to loop and a pattern break occurs we should stay on the same pattern.
						//Use nPosJump to force playback to "jump to this pattern" rather than move to next, as by default.
						//rewbs.to
						nPosJump = m_PlayState.m_nCurrentOrder;
					}
				}
			}
			break;

		// IMF / PTM Note Slides
		case CMD_NOTESLIDEUP:
		case CMD_NOTESLIDEDOWN:
		case CMD_NOTESLIDEUPRETRIG:
		case CMD_NOTESLIDEDOWNRETRIG:
			// Note that this command seems to be a bit buggy in Polytracker... Luckily, no tune seems to seriously use this
			// (Vic uses it e.g. in Spaceman or Perfect Reason to slide effect samples, noone will notice the difference :)
			NoteSlide(pChn, param, cmd == CMD_NOTESLIDEUP || cmd == CMD_NOTESLIDEUPRETRIG, cmd == CMD_NOTESLIDEUPRETRIG || cmd == CMD_NOTESLIDEDOWNRETRIG);
			break;

		// PTM Reverse sample + offset (executed on every tick)
		case CMD_REVERSEOFFSET:
			ReverseSampleOffset(*pChn, static_cast<ModCommand::PARAM>(param));
			break;

#ifndef NO_PLUGINS
		// DBM: Toggle DSP Echo
		case CMD_DBMECHO:
			if(m_PlayState.m_nTickCount == 0)
			{
				uint32 chns = (param >> 4), enable = (param & 0x0F);
				if(chns > 1 || enable > 2)
				{
					break;
				}
				CHANNELINDEX firstChn = nChn, lastChn = nChn;
				if(chns == 1)
				{
					firstChn = 0;
					lastChn = m_nChannels - 1;
				}
				for(CHANNELINDEX c = firstChn; c <= lastChn; c++)
				{
					ChnSettings[c].dwFlags.set(CHN_NOFX, enable == 1);
					m_PlayState.Chn[c].dwFlags.set(CHN_NOFX, enable == 1);
				}
			}
			break;
#endif // NO_PLUGINS
		}

		if(m_playBehaviour[kST3EffectMemory] && param != 0)
		{
			UpdateS3MEffectMemory(pChn, static_cast<ModCommand::PARAM>(param));
		}

		if(pChn->rowCommand.instr)
		{
			// Not necessarily consistent with actually playing instrument for IT compatibility
			pChn->nOldIns = pChn->rowCommand.instr;
		}

	} // for(...) end

	// Navigation Effects
	if(m_SongFlags[SONG_FIRSTTICK])
	{
		const bool doPatternLoop = (nPatLoopRow != ROWINDEX_INVALID);
		const bool doBreakRow = (nBreakRow != ROWINDEX_INVALID);
		const bool doPosJump = (nPosJump != ORDERINDEX_INVALID);

		// Pattern Loop
		if(doPatternLoop)
		{
			m_PlayState.m_nNextOrder = m_PlayState.m_nCurrentOrder;
			m_PlayState.m_nNextRow = nPatLoopRow;
			if(m_PlayState.m_nPatternDelay)
			{
				m_PlayState.m_nNextRow++;
			}

			// IT Compatibility: If the restart row is past the end of the current pattern
			// (e.g. when continued from a previous pattern without explicit SB0 effect), continue the next pattern.
			// Test case: LoopStartAfterPatternEnd.it
			if(nPatLoopRow >= Patterns[m_PlayState.m_nPattern].GetNumRows())
			{
				m_PlayState.m_nNextOrder++;
				m_PlayState.m_nNextRow = 0;
			}

			// As long as the pattern loop is running, mark the looped rows as not visited yet
			visitedSongRows.ResetPatternLoop(m_PlayState.m_nCurrentOrder, nPatLoopRow);
		}

		// Pattern Break / Position Jump only if no loop running
		// Exception: FastTracker 2 in all cases, Impulse Tracker in case of position jump
		// Test case for FT2 exception: PatLoop-Jumps.xm, PatLoop-Various.xm
		// Test case for IT: exception: LoopBreak.it
		if((doBreakRow || doPosJump)
			&& (!doPatternLoop || m_playBehaviour[kFT2PatternLoopWithJumps] || (m_playBehaviour[kITPatternLoopWithJumps] && doPosJump)))
		{
			if(!doPosJump) nPosJump = m_PlayState.m_nCurrentOrder + 1;
			if(!doBreakRow) nBreakRow = 0;
			m_SongFlags.set(SONG_BREAKTOROW);

			if(nPosJump >= Order().size())
			{
				nPosJump = Order().GetRestartPos();
			}

			// IT / FT2 compatibility: don't reset loop count on pattern break.
			// Test case: gm-trippy01.it, PatLoop-Break.xm, PatLoop-Weird.xm, PatLoop-Break.mod
			if(nPosJump != m_PlayState.m_nCurrentOrder
				&& !m_playBehaviour[kITPatternLoopBreak] && !m_playBehaviour[kFT2PatternLoopWithJumps] && GetType() != MOD_TYPE_MOD)
			{
				for(CHANNELINDEX i = 0; i < GetNumChannels(); i++)
				{
					m_PlayState.Chn[i].nPatternLoopCount = 0;
				}
			}

			m_PlayState.m_nNextRow = nBreakRow;
			if(!m_SongFlags[SONG_PATTERNLOOP])
				m_PlayState.m_nNextOrder = nPosJump;
		}

	}
	return true;
}