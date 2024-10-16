TEMPO CSoundFile::ConvertST2Tempo(uint8 tempo)
{
	static const uint8 ST2TempoFactor[] = { 140, 50, 25, 15, 10, 7, 6, 4, 3, 3, 2, 2, 2, 2, 1, 1 };
	static const uint32 st2MixingRate = 23863; // Highest possible setting in ST2

	// This underflows at tempo 06...0F, and the resulting tick lengths depend on the mixing rate.
	int32 samplesPerTick = st2MixingRate / (49 - ((ST2TempoFactor[tempo >> 4u] * (tempo & 0x0F)) >> 4u));
	if(samplesPerTick <= 0)
		samplesPerTick += 65536;
	return TEMPO().SetRaw(Util::muldivrfloor(st2MixingRate, 5 * TEMPO::fractFact, samplesPerTick * 2));
}