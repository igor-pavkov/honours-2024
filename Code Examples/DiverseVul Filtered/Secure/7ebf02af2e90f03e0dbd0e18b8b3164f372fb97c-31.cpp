static uint32 GetLinearSlideUpTable      (const CSoundFile *sndFile, uint32 i) { MPT_ASSERT(i < CountOf(LinearSlideDownTable));     return sndFile->m_playBehaviour[kHertzInLinearMode] ? LinearSlideUpTable[i]       : LinearSlideDownTable[i]; }