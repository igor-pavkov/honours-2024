void CSoundFile::RetrigNote(CHANNELINDEX nChn, int param, int offset)
{
	// Retrig: bit 8 is set if it's the new XM retrig
	ModChannel &chn = m_PlayState.Chn[nChn];
	int retrigSpeed = param & 0x0F;
	int16 retrigCount = chn.nRetrigCount;
	bool doRetrig = false;

	// IT compatibility 15. Retrigger
	if(m_playBehaviour[kITRetrigger])
	{
		if(m_PlayState.m_nTickCount == 0 && chn.rowCommand.note)
		{
			chn.nRetrigCount = param & 0xf;
		} else if(!chn.nRetrigCount || !--chn.nRetrigCount)
		{
			chn.nRetrigCount = param & 0xf;
			doRetrig = true;
		}
	} else if(m_playBehaviour[kFT2Retrigger] && (param & 0x100))
	{
		// Buggy-like-hell FT2 Rxy retrig!
		// Test case: retrig.xm
		if(m_SongFlags[SONG_FIRSTTICK])
		{
			// Here are some really stupid things FT2 does on the first tick.
			// Test case: RetrigTick0.xm
			if(chn.rowCommand.instr > 0 && chn.rowCommand.IsNoteOrEmpty()) retrigCount = 1;
			if(chn.rowCommand.volcmd == VOLCMD_VOLUME && chn.rowCommand.vol != 0)
			{
				// I guess this condition simply checked if the volume byte was != 0 in FT2.
				chn.nRetrigCount = retrigCount;
				return;
			}
		}
		if(retrigCount >= retrigSpeed)
		{
			if(!m_SongFlags[SONG_FIRSTTICK] || !chn.rowCommand.IsNote())
			{
				doRetrig = true;
				retrigCount = 0;
			}
		}
	} else
	{
		// old routines
		if (GetType() & (MOD_TYPE_S3M|MOD_TYPE_IT|MOD_TYPE_MPT))
		{
			if (!retrigSpeed) retrigSpeed = 1;
			if ((retrigCount) && (!(retrigCount % retrigSpeed))) doRetrig = true;
			retrigCount++;
		} else if(GetType() == MOD_TYPE_MTM)
		{
			// In MultiTracker, E9x retriggers the last note at exactly the x-th tick of the row
			doRetrig = m_PlayState.m_nTickCount == static_cast<uint32>(param & 0x0F) && retrigSpeed != 0;
		} else
		{
			int realspeed = retrigSpeed;
			// FT2 bug: if a retrig (Rxy) occurs together with a volume command, the first retrig interval is increased by one tick
			if ((param & 0x100) && (chn.rowCommand.volcmd == VOLCMD_VOLUME) && (chn.rowCommand.param & 0xF0)) realspeed++;
			if(!m_SongFlags[SONG_FIRSTTICK] || (param & 0x100))
			{
				if (!realspeed) realspeed = 1;
				if ((!(param & 0x100)) && (m_PlayState.m_nMusicSpeed) && (!(m_PlayState.m_nTickCount % realspeed))) doRetrig = true;
				retrigCount++;
			} else if (GetType() & (MOD_TYPE_XM|MOD_TYPE_MT2)) retrigCount = 0;
			if (retrigCount >= realspeed)
			{
				if ((m_PlayState.m_nTickCount) || ((param & 0x100) && (!chn.rowCommand.note))) doRetrig = true;
			}
			if(m_playBehaviour[kFT2Retrigger] && param == 0)
			{
				// E90 = Retrig instantly, and only once
				doRetrig = (m_PlayState.m_nTickCount == 0);
			}
		}
	}

	// IT compatibility: If a sample is shorter than the retrig time (i.e. it stops before the retrig counter hits zero), it is not retriggered.
	// Test case: retrig-short.it
	if(chn.nLength == 0 && m_playBehaviour[kITShortSampleRetrig] && !chn.HasMIDIOutput())
	{
		return;
	}

	if(doRetrig)
	{
		uint32 dv = (param >> 4) & 0x0F;
		int vol = chn.nVolume;
		if (dv)
		{

			// FT2 compatibility: Retrig + volume will not change volume of retrigged notes
			if(!m_playBehaviour[kFT2Retrigger] || !(chn.rowCommand.volcmd == VOLCMD_VOLUME))
			{
				if (retrigTable1[dv])
					vol = (vol * retrigTable1[dv]) >> 4;
				else
					vol += ((int)retrigTable2[dv]) << 2;
			}
			Limit(vol, 0, 256);

			chn.dwFlags.set(CHN_FASTVOLRAMP);
		}
		uint32 note = chn.nNewNote;
		int32 oldPeriod = chn.nPeriod;
		if (note >= NOTE_MIN && note <= NOTE_MAX && chn.nLength)
			CheckNNA(nChn, 0, note, true);
		bool resetEnv = false;
		if(GetType() & (MOD_TYPE_XM | MOD_TYPE_MT2))
		{
			if((chn.rowCommand.instr) && (param < 0x100))
			{
				InstrumentChange(&chn, chn.rowCommand.instr, false, false);
				resetEnv = true;
			}
			if (param < 0x100) resetEnv = true;
		}
		bool fading = chn.dwFlags[CHN_NOTEFADE];
		// IT compatibility: Really weird combination of envelopes and retrigger (see Storlek's q.it testcase)
		// Test case: retrig.it
		NoteChange(&chn, note, m_playBehaviour[kITRetrigger], resetEnv);
		// XM compatibility: Prevent NoteChange from resetting the fade flag in case an instrument number + note-off is present.
		// Test case: RetrigFade.xm
		if(fading && GetType() == MOD_TYPE_XM)
			chn.dwFlags.set(CHN_NOTEFADE);
		chn.nVolume = vol;
		if(m_nInstruments)
		{
			chn.rowCommand.note = static_cast<ModCommand::NOTE>(note);	// No retrig without note...
#ifndef NO_PLUGINS
			ProcessMidiOut(nChn);	//Send retrig to Midi
#endif // NO_PLUGINS
		}
		if ((GetType() & (MOD_TYPE_IT|MOD_TYPE_MPT)) && (!chn.rowCommand.note) && (oldPeriod)) chn.nPeriod = oldPeriod;
		if (!(GetType() & (MOD_TYPE_S3M|MOD_TYPE_IT|MOD_TYPE_MPT))) retrigCount = 0;
		// IT compatibility: see previous IT compatibility comment =)
		if(m_playBehaviour[kITRetrigger]) chn.position.Set(0);

		offset--;
		if(offset >= 0 && offset <= static_cast<int>(CountOf(chn.pModSample->cues)) && chn.pModSample != nullptr)
		{
			if(offset == 0) offset = chn.oldOffset;
			else offset = chn.oldOffset = chn.pModSample->cues[offset - 1];
			SampleOffset(chn, offset);
		}
	}

	// buggy-like-hell FT2 Rxy retrig!
	if(m_playBehaviour[kFT2Retrigger] && (param & 0x100)) retrigCount++;

	// Now we can also store the retrig value for IT...
	if(!m_playBehaviour[kITRetrigger])
		chn.nRetrigCount = retrigCount;
}