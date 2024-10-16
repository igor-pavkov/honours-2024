gdImagePtr gdImageCreateFromGifSource(gdSourcePtr inSource) /* {{{ */
{
	gdIOCtx         *in = gdNewSSCtx(inSource, NULL);
	gdImagePtr      im;

	im = gdImageCreateFromGifCtx(in);

	in->gd_free(in);

	return im;
}