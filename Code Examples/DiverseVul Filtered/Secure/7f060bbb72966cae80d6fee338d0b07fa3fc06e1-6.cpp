static int avi_write_data(avi_t *AVI, char *data, unsigned int length, int audio, int keyframe)
{
	int n = 0;

	unsigned char astr[5];

	// transcode core itself checks for the size -- unneeded and
	// does harm to xvid 2pass encodes where the first pass can get
	// _very_ large -- tibit.

#if 0
	/* Check for maximum file length */

	if ( (AVI->pos + 8 + length + 8 + (AVI->n_idx+1)*16) > AVI_MAX_LEN ) {
		AVI_errno = AVI_ERR_SIZELIM;
		return -1;
	}
#endif

	/* Add index entry */

	//set tag for current audio track
	sprintf((char *)astr, "0%1dwb", (int)(AVI->aptr+1));

	if(audio) {
		if (!AVI->is_opendml) n = avi_add_index_entry(AVI,astr,0x10,AVI->pos,length);
		n += avi_add_odml_index_entry(AVI,astr,0x10,AVI->pos,length);
	} else {
		if (!AVI->is_opendml) n = avi_add_index_entry(AVI,(unsigned char *)"00db",((keyframe)?0x10:0x0),AVI->pos,length);
		n += avi_add_odml_index_entry(AVI,(unsigned char *)"00db",((keyframe)?0x10:0x0),AVI->pos,length);
	}

	if(n) return -1;

	/* Output tag and data */

	if(audio)
		n = avi_add_chunk(AVI,(unsigned char *)astr, (unsigned char *)data, length);
	else
		n = avi_add_chunk(AVI,(unsigned char *)"00db", (unsigned char *)data, length);

	if (n) return -1;

	return 0;
}