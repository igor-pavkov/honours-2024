void CSoundFile::FineVibrato(ModChannel *p, uint32 param) const
{
	if (param & 0x0F) p->nVibratoDepth = param & 0x0F;
	if (param & 0xF0) p->nVibratoSpeed = (param >> 4) & 0x0F;
	p->dwFlags.set(CHN_VIBRATO);
	// ST3 compatibility: Do not distinguish between vibrato types in effect memory
	// Test case: VibratoTypeChange.s3m
	if(m_playBehaviour[kST3VibratoMemory] && (param & 0x0F))
	{
		p->nVibratoDepth *= 4u;
	}
}