gdImagePtr gdImageCreateFromGif(FILE *fdFile) /* {{{ */
{
	gdIOCtx		*fd = gdNewFileCtx(fdFile);
	gdImagePtr    	im = 0;

	im = gdImageCreateFromGifCtx(fd);

	fd->gd_free(fd);

	return im;
}