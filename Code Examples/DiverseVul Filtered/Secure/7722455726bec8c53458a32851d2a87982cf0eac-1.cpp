void gdImageGd2 (gdImagePtr im, FILE * outFile, int cs, int fmt)
{
	gdIOCtx *out = gdNewFileCtx(outFile);

	_gdImageGd2(im, out, cs, fmt);

	out->gd_free(out);
}