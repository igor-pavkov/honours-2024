uint32 CSoundFile::GetFreqFromPeriod(uint32 period, uint32 c5speed, int32 nPeriodFrac) const
{
	if (!period) return 0;
	if (GetType() == MOD_TYPE_XM)
	{
		if(m_playBehaviour[kFT2Periods])
		{
			// FT2 compatibility: Period is a 16-bit value in FT2, and it overflows happily.
			// Test case: FreqWraparound.xm
			period &= 0xFFFF;
		}
		if(m_SongFlags[SONG_LINEARSLIDES])
		{
			uint32 octave;
			if(m_playBehaviour[kFT2Periods])
			{
				// Under normal circumstances, this calculation returns the same values as the non-compatible one.
				// However, once the 12 octaves are exceeded (through portamento slides), the octave shift goes
				// crazy in FT2, meaning that the frequency wraps around randomly...
				// The entries in FT2's conversion table are four times as big, hence we have to do an additional shift by two bits.
				// Test case: FreqWraparound.xm
				// 12 octaves * (12 * 64) LUT entries = 9216, add 767 for rounding
				uint32 div = ((9216u + 767u - period) / 768);
				octave = ((14 - div) & 0x1F);
			} else
			{
				octave = (period / 768) + 2;
			}
			return (XMLinearTable[period % 768] << (FREQ_FRACBITS + 2)) >> octave;
		} else
		{
			if(!period) period = 1;
			return ((8363 * 1712L) << FREQ_FRACBITS) / period;
		}
	} else if (UseFinetuneAndTranspose())
	{
		return ((3546895L * 4) << FREQ_FRACBITS) / period;
	} else if(GetType() == MOD_TYPE_669)
	{
		// We only really use c5speed for the finetune pattern command. All samples in 669 files have the same middle-C speed (imported as 8363 Hz).
		return (period + c5speed - 8363) << FREQ_FRACBITS;
	} else if(GetType() & (MOD_TYPE_MDL | MOD_TYPE_DTM))
	{
		LimitMax(period, Util::MaxValueOfType(period) >> 8);
		if (!c5speed) c5speed = 8363;
		return Util::muldiv_unsigned(c5speed, (1712L << 7) << FREQ_FRACBITS, (period << 8) + nPeriodFrac);
	} else
	{
		LimitMax(period, Util::MaxValueOfType(period) >> 8);
		if(m_SongFlags[SONG_LINEARSLIDES])
		{
			if(m_playBehaviour[kHertzInLinearMode])
			{
				// IT linear slides already use frequencies instead of periods.
				static_assert(FREQ_FRACBITS <= 8, "Check this shift operator");
				return uint32(((uint64(period) << 8) + nPeriodFrac) >> (8 - FREQ_FRACBITS));
			} else
			{
				if (!c5speed) c5speed = 8363;
				return Util::muldiv_unsigned(c5speed, (1712L << 8) << FREQ_FRACBITS, (period << 8) + nPeriodFrac);
			}
		} else
		{
			return Util::muldiv_unsigned(8363, (1712L << 8) << FREQ_FRACBITS, (period << 8) + nPeriodFrac);
		}
	}
}