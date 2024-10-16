int AVI_append_audio(avi_t *AVI, u8 *data, int bytes)
{

	// won't work for >2gb
	int i, length, pos;
	unsigned char c[4];

	if(AVI->mode==AVI_MODE_READ) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}

	// update last index entry:

	--AVI->n_idx;
	length = str2ulong(AVI->idx[AVI->n_idx]+12);
	pos    = str2ulong(AVI->idx[AVI->n_idx]+8);

	//update;
	long2str(AVI->idx[AVI->n_idx]+12,length+bytes);

	++AVI->n_idx;

	AVI->track[AVI->aptr].audio_bytes += bytes;

	//update chunk header
	gf_fseek(AVI->fdes, pos+4, SEEK_SET);
	long2str(c, length+bytes);
	avi_write(AVI->fdes, (char *)c, 4);

	gf_fseek(AVI->fdes, pos+8+length, SEEK_SET);

	i=PAD_EVEN(length + bytes);

	bytes = i - length;
	avi_write(AVI->fdes, data, bytes);
	AVI->pos = pos + 8 + i;

	return 0;
}