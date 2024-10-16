void CSoundFile::ExtendedMODCommands(CHANNELINDEX nChn, ModCommand::PARAM param)
{
	ModChannel *pChn = &m_PlayState.Chn[nChn];
	uint8 command = param & 0xF0;
	param &= 0x0F;
	switch(command)
	{
	// E0x: Set Filter
	case 0x00:
		for(CHANNELINDEX chn = 0; chn < GetNumChannels(); chn++)
		{
			m_PlayState.Chn[chn].dwFlags.set(CHN_AMIGAFILTER, !(param & 1));
		}
		break;
	// E1x: Fine Portamento Up
	case 0x10:	if ((param) || (GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2))) FinePortamentoUp(pChn, param); break;
	// E2x: Fine Portamento Down
	case 0x20:	if ((param) || (GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2))) FinePortamentoDown(pChn, param); break;
	// E3x: Set Glissando Control
	case 0x30:	pChn->dwFlags.set(CHN_GLISSANDO, param != 0); break;
	// E4x: Set Vibrato WaveForm
	case 0x40:	pChn->nVibratoType = param & 0x07; break;
	// E5x: Set FineTune
	case 0x50:	if(!m_SongFlags[SONG_FIRSTTICK])
				{
					break;
				}
				if(GetType() & (MOD_TYPE_MOD | MOD_TYPE_DIGI | MOD_TYPE_AMF0 | MOD_TYPE_MED))
				{
					pChn->nFineTune = MOD2XMFineTune(param);
					if(pChn->nPeriod && pChn->rowCommand.IsNote()) pChn->nPeriod = GetPeriodFromNote(pChn->nNote, pChn->nFineTune, pChn->nC5Speed);
				} else if(pChn->rowCommand.IsNote())
				{
					pChn->nFineTune = MOD2XMFineTune(param - 8);
					if(pChn->nPeriod) pChn->nPeriod = GetPeriodFromNote(pChn->nNote, pChn->nFineTune, pChn->nC5Speed);
				}
				break;
	// E6x: Pattern Loop
	// E7x: Set Tremolo WaveForm
	case 0x70:	pChn->nTremoloType = param & 0x07; break;
	// E8x: Set 4-bit Panning
	case 0x80:
		if(m_SongFlags[SONG_FIRSTTICK])
		{
			Panning(pChn, param, Pan4bit);
		}
		break;
	// E9x: Retrig
	case 0x90:	RetrigNote(nChn, param); break;
	// EAx: Fine Volume Up
	case 0xA0:	if ((param) || (GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2))) FineVolumeUp(pChn, param, false); break;
	// EBx: Fine Volume Down
	case 0xB0:	if ((param) || (GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2))) FineVolumeDown(pChn, param, false); break;
	// ECx: Note Cut
	case 0xC0:	NoteCut(nChn, param, false); break;
	// EDx: Note Delay
	// EEx: Pattern Delay
	case 0xF0:
		if(GetType() == MOD_TYPE_MOD) // MOD: Invert Loop
		{
			pChn->nEFxSpeed = param;
			if(m_SongFlags[SONG_FIRSTTICK]) InvertLoop(pChn);
		} else // XM: Set Active Midi Macro
		{
			pChn->nActiveMacro = param;
		}
		break;
	}
}