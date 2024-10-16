uint32 CSoundFile::GetPeriodFromNote(uint32 note, int32 nFineTune, uint32 nC5Speed) const
{
	if (note == NOTE_NONE || (note >= NOTE_MIN_SPECIAL)) return 0;
	note -= NOTE_MIN;
	if (!UseFinetuneAndTranspose())
	{
		if(GetType() & (MOD_TYPE_MDL | MOD_TYPE_DTM))
		{
			// MDL uses non-linear slides, but their effectiveness does not depend on the middle-C frequency.
			return (FreqS3MTable[note % 12u] << 4) >> (note / 12);
		}
		if(m_SongFlags[SONG_LINEARSLIDES] || GetType() == MOD_TYPE_669)
		{
			// In IT linear slide mode, directly use frequency in Hertz rather than periods.
			if(m_playBehaviour[kHertzInLinearMode] || GetType() == MOD_TYPE_669)
				return Util::muldiv_unsigned(nC5Speed, LinearSlideUpTable[(note % 12u) * 16u] << (note / 12u), 65536 << 5);
			else
				return (FreqS3MTable[note % 12u] << 5) >> (note / 12);
		} else
		{
			if (!nC5Speed)
				nC5Speed = 8363;
			LimitMax(nC5Speed, uint32_max >> (note / 12u));
			//(a*b)/c
			return Util::muldiv_unsigned(8363, (FreqS3MTable[note % 12u] << 5), nC5Speed << (note / 12u));
			//8363 * freq[note%12] / nC5Speed * 2^(5-note/12)
		}
	} else if (GetType() == MOD_TYPE_XM)
	{
		if (note < 12) note = 12;
		note -= 12;

		// FT2 Compatibility: The lower three bits of the finetune are truncated.
		// Test case: Finetune-Precision.xm
		if(m_playBehaviour[kFT2FinetunePrecision])
		{
			nFineTune &= ~7;
		}

		if(m_SongFlags[SONG_LINEARSLIDES])
		{
			int l = ((NOTE_MAX - note) << 6) - (nFineTune / 2);
			if (l < 1) l = 1;
			return static_cast<uint32>(l);
		} else
		{
			int finetune = nFineTune;
			uint32 rnote = (note % 12) << 3;
			uint32 roct = note / 12;
			int rfine = finetune / 16;
			int i = rnote + rfine + 8;
			Limit(i , 0, 103);
			uint32 per1 = XMPeriodTable[i];
			if(finetune < 0)
			{
				rfine--;
				finetune = -finetune;
			} else rfine++;
			i = rnote+rfine+8;
			if (i < 0) i = 0;
			if (i >= 104) i = 103;
			uint32 per2 = XMPeriodTable[i];
			rfine = finetune & 0x0F;
			per1 *= 16-rfine;
			per2 *= rfine;
			return ((per1 + per2) << 1) >> roct;
		}
	} else
	{
		nFineTune = XM2MODFineTune(nFineTune);
		if ((nFineTune) || (note < 36) || (note >= 36 + 6 * 12))
			return (ProTrackerTunedPeriods[nFineTune * 12u + note % 12u] << 5) >> (note / 12u);
		else
			return (ProTrackerPeriodTable[note - 36] << 2);
	}
}