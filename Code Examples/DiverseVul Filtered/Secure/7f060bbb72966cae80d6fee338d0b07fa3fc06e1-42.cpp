void AVI_set_audio_vbr(avi_t *AVI, int is_vbr)
{
	AVI->track[AVI->aptr].a_vbr = is_vbr;
}