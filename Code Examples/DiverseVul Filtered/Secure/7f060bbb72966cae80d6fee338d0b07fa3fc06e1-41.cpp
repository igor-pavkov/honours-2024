int AVI_audio_rate(avi_t *AVI)
{
	return AVI->track[AVI->aptr].a_rate;
}