u64 AVI_bytes_remain(avi_t *AVI)
{
	if(AVI->mode==AVI_MODE_READ) return 0;

	return ( AVI_MAX_LEN - (AVI->pos + 8 + 16*AVI->n_idx));
}