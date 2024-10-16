int AVI_write_frame(avi_t *AVI, u8 *data, int bytes, int keyframe)
{
	s64 pos;

	if(AVI->mode==AVI_MODE_READ) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}

	pos = AVI->pos;

	if(avi_write_data(AVI,data,bytes,0,keyframe)) return -1;

	AVI->last_pos = pos;
	AVI->last_len = bytes;
	AVI->video_frames++;
	return 0;
}