uint32 CSoundFile::SendMIDIData(CHANNELINDEX nChn, bool isSmooth, const unsigned char *macro, uint32 macroLen, PLUGINDEX plugin)
{
	if(macroLen < 1)
	{
		return 0;
	}

	if(macro[0] == 0xFA || macro[0] == 0xFC || macro[0] == 0xFF)
	{
		// Start Song, Stop Song, MIDI Reset - both interpreted internally and sent to plugins
		for(CHANNELINDEX chn = 0; chn < GetNumChannels(); chn++)
		{
			m_PlayState.Chn[chn].nCutOff = 0x7F;
			m_PlayState.Chn[chn].nResonance = 0x00;
		}
	}

	ModChannel &chn = m_PlayState.Chn[nChn];
	if(macro[0] == 0xF0 && (macro[1] == 0xF0 || macro[1] == 0xF1))
	{
		// Internal device.
		if(macroLen < 4)
		{
			return 0;
		}
		const bool isExtended = (macro[1] == 0xF1);
		const uint8 macroCode = macro[2];
		const uint8 param = macro[3];

		if(macroCode == 0x00 && !isExtended && param < 0x80)
		{
			// F0.F0.00.xx: Set CutOff
			if(!isSmooth)
			{
				chn.nCutOff = param;
			} else
			{
				chn.nCutOff = Util::Round<uint8>(CalculateSmoothParamChange(chn.nCutOff, param));
			}
			chn.nRestoreCutoffOnNewNote = 0;
			SetupChannelFilter(&chn, !chn.dwFlags[CHN_FILTER]);

			return 4;
		} else if(macroCode == 0x01 && !isExtended && param < 0x80)
		{
			// F0.F0.01.xx: Set Resonance
			if(!isSmooth)
			{
				chn.nResonance = param;
			} else
			{
				chn.nResonance = (uint8)CalculateSmoothParamChange((float)chn.nResonance, (float)param);
			}
			chn.nRestoreResonanceOnNewNote = 0;
			SetupChannelFilter(&chn, !chn.dwFlags[CHN_FILTER]);

			return 4;
		} else if(macroCode == 0x02 && !isExtended)
		{
			// F0.F0.02.xx: Set filter mode (high nibble determines filter mode)
			if(param < 0x20)
			{
				chn.nFilterMode = (param >> 4);
				SetupChannelFilter(&chn, !chn.dwFlags[CHN_FILTER]);
			}

			return 4;
#ifndef NO_PLUGINS
		} else if(macroCode == 0x03 && !isExtended)
		{
			// F0.F0.03.xx: Set plug dry/wet
			const PLUGINDEX nPlug = (plugin != 0) ? plugin : GetBestPlugin(nChn, PrioritiseChannel, EvenIfMuted);
			if ((nPlug) && (nPlug <= MAX_MIXPLUGINS) && param < 0x80)
			{
				const float newRatio = (0x7F - (param & 0x7F)) / 127.0f;
				if(!isSmooth)
				{
					m_MixPlugins[nPlug - 1].fDryRatio = newRatio;
				} else
				{
					m_MixPlugins[nPlug - 1].fDryRatio = CalculateSmoothParamChange(m_MixPlugins[nPlug - 1].fDryRatio, newRatio);
				}
			}

			return 4;
		} else if((macroCode & 0x80) || isExtended)
		{
			// F0.F0.{80|n}.xx / F0.F1.n.xx: Set VST effect parameter n to xx
			const PLUGINDEX nPlug = (plugin != 0) ? plugin : GetBestPlugin(nChn, PrioritiseChannel, EvenIfMuted);
			const uint32 plugParam = isExtended ? (0x80 + macroCode) : (macroCode & 0x7F);
			if((nPlug) && (nPlug <= MAX_MIXPLUGINS))
			{
				IMixPlugin *pPlugin = m_MixPlugins[nPlug - 1].pMixPlugin;
				if(pPlugin && param < 0x80)
				{
					const float fParam = param / 127.0f;
					if(!isSmooth)
					{
						pPlugin->SetParameter(plugParam, fParam);
					} else
					{
						pPlugin->SetParameter(plugParam, CalculateSmoothParamChange(pPlugin->GetParameter(plugParam), fParam));
					}
				}
			}

			return 4;
#endif // NO_PLUGINS
		}

		// If we reach this point, the internal macro was invalid.

	} else
	{
#ifndef NO_PLUGINS
		// Not an internal device. Pass on to appropriate plugin.
		const CHANNELINDEX plugChannel = (nChn < GetNumChannels()) ? nChn + 1 : chn.nMasterChn;
		if(plugChannel > 0 && plugChannel <= GetNumChannels())	// XXX do we need this? I guess it might be relevant for previewing notes in the pattern... Or when using this mechanism for volume/panning!
		{
			PLUGINDEX nPlug = 0;
			if(!chn.dwFlags[CHN_NOFX])
			{
				nPlug = (plugin != 0) ? plugin : GetBestPlugin(nChn, PrioritiseChannel, EvenIfMuted);
			}

			if(nPlug > 0 && nPlug <= MAX_MIXPLUGINS)
			{
				IMixPlugin *pPlugin = m_MixPlugins[nPlug - 1].pMixPlugin;
				if (pPlugin != nullptr)
				{
					if(macro[0] == 0xF0)
					{
						pPlugin->MidiSysexSend(macro, macroLen);
					} else
					{
						uint32 len = std::min<uint32>(MIDIEvents::GetEventLength(macro[0]), macroLen);
						uint32 curData = 0;
						memcpy(&curData, macro, len);
						pPlugin->MidiSend(curData);
					}
				}
			}
		}
#else
		MPT_UNREFERENCED_PARAMETER(plugin);
#endif // NO_PLUGINS

		return macroLen;

	}

	return 0;

}