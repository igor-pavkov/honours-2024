static int avi_add_index_entry(avi_t *AVI, unsigned char *tag, int flags, u64 pos, u64 len)
{
	if(AVI->n_idx>=AVI->max_idx) {
		void *ptr = gf_realloc((void *)AVI->idx,(AVI->max_idx+4096)*16);

		if(ptr == 0) {
			AVI_errno = AVI_ERR_NO_MEM;
			return -1;
		}
		AVI->max_idx += 4096;
		AVI->idx = (unsigned char((*)[16]) ) ptr;
	}

	/* Add index entry */

	//   GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] INDEX %s %ld %lu %lu\n", tag, flags, pos, len));

	memcpy(AVI->idx[AVI->n_idx],tag,4);
	long2str(AVI->idx[AVI->n_idx]+ 4,flags);
	long2str(AVI->idx[AVI->n_idx]+ 8, (s32) pos);
	long2str(AVI->idx[AVI->n_idx]+12, (s32) len);

	/* Update counter */

	AVI->n_idx++;

	if(len>AVI->max_len) AVI->max_len=(u32) len;

	return 0;
}