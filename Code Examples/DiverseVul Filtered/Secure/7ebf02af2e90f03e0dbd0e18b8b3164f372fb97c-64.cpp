	GetLengthMemory(const CSoundFile &sf)
		: sndFile(sf)
		, state(mpt::make_unique<CSoundFile::PlayState>(sf.m_PlayState))
	{
		Reset();
	}