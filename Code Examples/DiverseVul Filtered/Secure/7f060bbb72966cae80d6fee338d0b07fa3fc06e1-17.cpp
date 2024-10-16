int AVI_get_audio_vbr(avi_t *AVI)
{
	return(AVI->track[AVI->aptr].a_vbr);
}