int AVI_set_audio_position(avi_t *AVI, int byte)
{
	int n0, n1;

	if(AVI->mode==AVI_MODE_WRITE) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}
	if(!AVI->track[AVI->aptr].audio_index)         {
		AVI_errno = AVI_ERR_NO_IDX;
		return -1;
	}

	if(byte < 0) byte = 0;

	/* Binary search in the audio chunks */

	n0 = 0;
	n1 = AVI->track[AVI->aptr].audio_chunks;

	while(n0<n1-1)
	{
		int n = (n0+n1)/2;
		if(AVI->track[AVI->aptr].audio_index[n].tot>(u32) byte)
			n1 = n;
		else
			n0 = n;
	}

	AVI->track[AVI->aptr].audio_posc = n0;
	AVI->track[AVI->aptr].audio_posb = (u32) (byte - AVI->track[AVI->aptr].audio_index[n0].tot);

	return 0;
}