void CSoundFile::Vibrato(ModChannel *p, uint32 param) const
{
	if (param & 0x0F) p->nVibratoDepth = (param & 0x0F) * 4;
	if (param & 0xF0) p->nVibratoSpeed = (param >> 4) & 0x0F;
	p->dwFlags.set(CHN_VIBRATO);
}