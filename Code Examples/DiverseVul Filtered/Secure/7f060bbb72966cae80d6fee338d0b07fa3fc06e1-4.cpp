int AVI_set_audio_position_index(avi_t *AVI, int indexpos)
{
	if(AVI->mode==AVI_MODE_WRITE) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}
	if(!AVI->track[AVI->aptr].audio_index)         {
		AVI_errno = AVI_ERR_NO_IDX;
		return -1;
	}
	if(indexpos > AVI->track[AVI->aptr].audio_chunks)     {
		AVI_errno = AVI_ERR_NO_IDX;
		return -1;
	}

	AVI->track[AVI->aptr].audio_posc = indexpos;
	AVI->track[AVI->aptr].audio_posb = 0;

	return 0;
}