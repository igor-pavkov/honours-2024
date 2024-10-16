void AVI_set_audio(avi_t *AVI, int channels, int rate, int bits, int format, int mp3rate)
{
	/* may only be called if file is open for writing */

	if(AVI->mode==AVI_MODE_READ) return;

	//inc audio tracks
	AVI->aptr=AVI->anum;
	++AVI->anum;

	if(AVI->anum > AVI_MAX_TRACKS) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[avilib] error - only %d audio tracks supported\n", AVI_MAX_TRACKS));
		exit(1);
	}

	AVI->track[AVI->aptr].a_chans = channels;
	AVI->track[AVI->aptr].a_rate  = rate;
	AVI->track[AVI->aptr].a_bits  = bits;
	AVI->track[AVI->aptr].a_fmt   = format;
	AVI->track[AVI->aptr].mp3rate = mp3rate;

	avi_update_header(AVI);
}