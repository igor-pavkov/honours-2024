int AVI_audio_bits(avi_t *AVI)
{
	return AVI->track[AVI->aptr].a_bits;
}