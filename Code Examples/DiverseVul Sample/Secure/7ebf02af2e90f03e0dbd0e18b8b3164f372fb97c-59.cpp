float CSoundFile::CalculateSmoothParamChange(float currentValue, float param) const
{
	MPT_ASSERT(GetNumTicksOnCurrentRow() > m_PlayState.m_nTickCount);
	const uint32 ticksLeft = GetNumTicksOnCurrentRow() - m_PlayState.m_nTickCount;
	if(ticksLeft > 1)
	{
		// Slide param
		const float step = (param - currentValue) / (float)ticksLeft;
		return (currentValue + step);
	} else
	{
		// On last tick, set exact value.
		return param;
	}
}