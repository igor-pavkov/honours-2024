int AVI_audio_format(avi_t *AVI)
{
	return AVI->track[AVI->aptr].a_fmt;
}