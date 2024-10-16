static void php_image_filter_pixelate(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *IM;
	gdImagePtr im;
	zend_long tmp, blocksize;
	zend_bool mode = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll|b", &IM, &tmp, &blocksize, &mode) == FAILURE) {
		RETURN_FALSE;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (im == NULL) {
		RETURN_FALSE;
	}

	if (gdImagePixelate(im, (int) blocksize, (const unsigned int) mode)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}