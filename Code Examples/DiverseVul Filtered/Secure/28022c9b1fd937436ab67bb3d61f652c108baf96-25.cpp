PHP_FUNCTION(imagecolorat)
{
	zval *IM;
	zend_long x, y;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll", &IM, &x, &y) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (gdImageTrueColor(im)) {
		if (im->tpixels && gdImageBoundsSafe(im, x, y)) {
			RETURN_LONG(gdImageTrueColorPixel(im, x, y));
		} else {
			php_error_docref(NULL, E_NOTICE, "%pd,%pd is out of bounds", x, y);
			RETURN_FALSE;
		}
	} else {
		if (im->pixels && gdImageBoundsSafe(im, x, y)) {
			RETURN_LONG(im->pixels[y][x]);
		} else {
			php_error_docref(NULL, E_NOTICE, "%pd,%pd is out of bounds", x, y);
			RETURN_FALSE;
		}
	}
}