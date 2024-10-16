avi_t *AVI_open_input_file(char *filename, int getIndex)
{
	avi_t *AVI;

	/* Create avi_t structure */

	AVI = (avi_t *) gf_malloc(sizeof(avi_t));
	if(AVI==NULL)
	{
		AVI_errno = AVI_ERR_NO_MEM;
		return 0;
	}
	memset((void *)AVI,0,sizeof(avi_t));

	AVI->mode = AVI_MODE_READ; /* open for reading */

	/* Open the file */

	AVI->fdes = gf_fopen(filename,"rb");
	if(!AVI->fdes )
	{
		AVI_errno = AVI_ERR_OPEN;
		gf_free(AVI);
		return 0;
	}

	AVI_errno = 0;
	avi_parse_input_file(AVI, getIndex);

	if (AVI != NULL && !AVI_errno) {
		AVI->aptr=0; //reset
	}

	if (AVI_errno) return NULL;

	return AVI;
}