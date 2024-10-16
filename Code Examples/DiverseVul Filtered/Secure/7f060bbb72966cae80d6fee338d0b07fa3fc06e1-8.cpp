int AVI_set_audio_bitrate(avi_t *AVI, int bitrate)
{
	if(AVI->mode==AVI_MODE_READ) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}

	AVI->track[AVI->aptr].mp3rate = bitrate;
	return 0;
}