u64 AVI_get_video_position(avi_t *AVI, int frame)
{
	if(AVI->mode==AVI_MODE_WRITE) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return (u64) -1;
	}
	if(!AVI->video_index)         {
		AVI_errno = AVI_ERR_NO_IDX;
		return (u64) -1;
	}

	if(frame < 0 || frame >= AVI->video_frames) return 0;
	return(AVI->video_index[frame].pos);
}