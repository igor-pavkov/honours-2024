int AVI_frame_size(avi_t *AVI, int frame)
{
	if(AVI->mode==AVI_MODE_WRITE) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}
	if(!AVI->video_index)         {
		AVI_errno = AVI_ERR_NO_IDX;
		return -1;
	}

	if(frame < 0 || frame >= AVI->video_frames) return 0;
	return (u32) (AVI->video_index[frame].len);
}