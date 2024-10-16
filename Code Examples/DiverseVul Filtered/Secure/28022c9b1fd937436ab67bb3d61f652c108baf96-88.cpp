PHP_FUNCTION(imagegammacorrect)
{
	zval *IM;
	gdImagePtr im;
	int i;
	double input, output;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rdd", &IM, &input, &output) == FAILURE) {
		return;
	}

	if ( input <= 0.0 || output <= 0.0 ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Gamma values should be positive");
		RETURN_FALSE;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (gdImageTrueColor(im))	{
		int x, y, c;

		for (y = 0; y < gdImageSY(im); y++)	{
			for (x = 0; x < gdImageSX(im); x++)	{
				c = gdImageGetPixel(im, x, y);
				gdImageSetPixel(im, x, y,
					gdTrueColorAlpha(
						(int) ((pow((pow((gdTrueColorGetRed(c)   / 255.0), input)), 1.0 / output) * 255) + .5),
						(int) ((pow((pow((gdTrueColorGetGreen(c) / 255.0), input)), 1.0 / output) * 255) + .5),
						(int) ((pow((pow((gdTrueColorGetBlue(c)  / 255.0), input)), 1.0 / output) * 255) + .5),
						gdTrueColorGetAlpha(c)
					)
				);
			}
		}
		RETURN_TRUE;
	}

	for (i = 0; i < gdImageColorsTotal(im); i++) {
		im->red[i]   = (int)((pow((pow((im->red[i]   / 255.0), input)), 1.0 / output) * 255) + .5);
		im->green[i] = (int)((pow((pow((im->green[i] / 255.0), input)), 1.0 / output) * 255) + .5);
		im->blue[i]  = (int)((pow((pow((im->blue[i]  / 255.0), input)), 1.0 / output) * 255) + .5);
	}

	RETURN_TRUE;
}