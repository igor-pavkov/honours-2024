gdImagePtr gdImageCreateFromGd2 (FILE * inFile)
{
	gdIOCtx *in = gdNewFileCtx(inFile);
	gdImagePtr im;

	im = gdImageCreateFromGd2Ctx(in);

	in->gd_free(in);

	return im;
}