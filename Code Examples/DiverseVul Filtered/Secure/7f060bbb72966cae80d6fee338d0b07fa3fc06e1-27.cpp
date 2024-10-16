int AVI_seek_start(avi_t *AVI)
{
	if(AVI->mode==AVI_MODE_WRITE) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}

	gf_fseek(AVI->fdes,AVI->movi_start,SEEK_SET);
	AVI->video_pos = 0;
	return 0;
}