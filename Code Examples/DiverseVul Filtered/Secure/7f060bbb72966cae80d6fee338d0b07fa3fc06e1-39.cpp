int AVI_write_audio(avi_t *AVI, u8 *data, int bytes)
{
	if(AVI->mode==AVI_MODE_READ) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}

	if( avi_write_data(AVI,data,bytes,1,0) ) return -1;
	AVI->track[AVI->aptr].audio_bytes += bytes;
	AVI->track[AVI->aptr].audio_chunks++;
	return 0;
}