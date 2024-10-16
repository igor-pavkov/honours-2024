static gdImagePtr _gd2CreateFromFile (gdIOCtxPtr in, int *sx, int *sy, int *cs, int *vers, int *fmt, int *ncx, int *ncy, t_chunk_info ** cidx)
{
	gdImagePtr im;

	if (_gd2GetHeader (in, sx, sy, cs, vers, fmt, ncx, ncy, cidx) != 1) {
		GD2_DBG(php_gd_error("Bad GD2 header"));
		goto fail1;
	}

	if (gd2_truecolor(*fmt)) {
		im = gdImageCreateTrueColor(*sx, *sy);
	} else {
		im = gdImageCreate(*sx, *sy);
	}
	if (im == NULL) {
		GD2_DBG(php_gd_error("Could not create gdImage"));
		goto fail1;
	}

	if (!_gdGetColors(in, im, (*vers) == 2)) {
		GD2_DBG(php_gd_error("Could not read color palette"));
		goto fail2;
	}
	GD2_DBG(php_gd_error("Image palette completed: %d colours", im->colorsTotal));

	return im;

fail2:
	gdImageDestroy(im);
	return 0;

fail1:
	return 0;
}