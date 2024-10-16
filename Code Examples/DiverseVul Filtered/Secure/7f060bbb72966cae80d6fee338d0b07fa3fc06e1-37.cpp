avi_t* AVI_open_output_file(char * filename, u64 opendml_threshold)
{
	avi_t *AVI;
	int i;

	unsigned char AVI_header[HEADERBYTES];

	/* Allocate the avi_t struct and zero it */

	AVI = (avi_t *) gf_malloc(sizeof(avi_t));
	if(AVI==0)
	{
		AVI_errno = AVI_ERR_NO_MEM;
		return 0;
	}
	memset((void *)AVI,0,sizeof(avi_t));

	AVI->fdes = gf_fopen(filename, "w+b");
	if (!AVI->fdes )
	{
		AVI_errno = AVI_ERR_OPEN;
		gf_free(AVI);
		return 0;
	}

	/* Write out HEADERBYTES bytes, the header will go here
	   when we are finished with writing */

	for (i=0; i<HEADERBYTES; i++) AVI_header[i] = 0;
	i = avi_write(AVI->fdes,(char *)AVI_header,HEADERBYTES);
	if (i != HEADERBYTES)
	{
		gf_fclose(AVI->fdes);
		AVI_errno = AVI_ERR_WRITE;
		gf_free(AVI);
		return 0;
	}

	AVI->pos  = HEADERBYTES;
	AVI->mode = AVI_MODE_WRITE; /* open for writing */
	if (opendml_threshold)
		AVI->new_riff_threshold = opendml_threshold;
	else
		AVI->new_riff_threshold = (1900*1024*1024);

	//init
	AVI->anum = 0;
	AVI->aptr = 0;

	return AVI;
}