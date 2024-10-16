int AVI_get_audio_position_index(avi_t *AVI)
{
	if(AVI->mode==AVI_MODE_WRITE) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}
	if(!AVI->track[AVI->aptr].audio_index) {
		AVI_errno = AVI_ERR_NO_IDX;
		return -1;
	}

	return (AVI->track[AVI->aptr].audio_posc);
}