static int avi_add_chunk(avi_t *AVI, unsigned char *tag, unsigned char *data, u32 length)
{
	unsigned char c[8];
	char p=0;

	/* Copy tag and length int c, so that we need only 1 write system call
	   for these two values */

	memcpy(c,tag,4);
	long2str(c+4,length);

	/* Output tag, length and data, restore previous position
	   if the write fails */

	if( avi_write(AVI->fdes,(char *)c,8) != 8 ||
	        avi_write(AVI->fdes,(char *)data,length) != length ||
	        avi_write(AVI->fdes,&p,length&1) != (length&1)) // if len is uneven, write a pad byte
	{
		gf_fseek(AVI->fdes,AVI->pos,SEEK_SET);
		AVI_errno = AVI_ERR_WRITE;
		return -1;
	}

	/* Update file position */

	AVI->pos += 8 + PAD_EVEN(length);

	//GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] pos=%lu %s\n", AVI->pos, tag));

	return 0;
}