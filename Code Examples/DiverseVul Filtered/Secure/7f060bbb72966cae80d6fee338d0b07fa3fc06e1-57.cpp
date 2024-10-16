u64 AVI_bytes_written(avi_t *AVI)
{
	if(AVI->mode==AVI_MODE_READ) return 0;

	return (AVI->pos + 8 + 16*AVI->n_idx);
}