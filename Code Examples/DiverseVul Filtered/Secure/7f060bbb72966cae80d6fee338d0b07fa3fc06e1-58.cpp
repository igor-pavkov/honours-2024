int AVI_audio_channels(avi_t *AVI)
{
	return AVI->track[AVI->aptr].a_chans;
}