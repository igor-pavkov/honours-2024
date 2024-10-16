int AVI_dup_frame(avi_t *AVI)
{
	if(AVI->mode==AVI_MODE_READ) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}

	if(AVI->last_pos==0) return 0; /* No previous real frame */
	if(avi_add_index_entry(AVI,(unsigned char *)"00db",0x10,AVI->last_pos,AVI->last_len)) return -1;
	AVI->video_frames++;
	AVI->must_use_index = 1;
	return 0;
}