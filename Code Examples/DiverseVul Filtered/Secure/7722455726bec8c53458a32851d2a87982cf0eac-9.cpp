gdImagePtr gdImageCreateFromGd2Ptr (int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	im = gdImageCreateFromGd2Ctx(in);
	in->gd_free(in);

	return im;
}