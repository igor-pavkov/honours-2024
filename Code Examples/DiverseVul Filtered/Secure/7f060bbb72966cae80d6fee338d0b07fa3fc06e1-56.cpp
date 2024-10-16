int AVI_audio_size(avi_t *AVI, int frame)
{
	if(AVI->mode==AVI_MODE_WRITE) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}
	if(!AVI->track[AVI->aptr].audio_index)         {
		AVI_errno = AVI_ERR_NO_IDX;
		return -1;
	}

	if(frame < 0 || frame >= AVI->track[AVI->aptr].audio_chunks) return -1;
	return (u32) (AVI->track[AVI->aptr].audio_index[frame].len);
}