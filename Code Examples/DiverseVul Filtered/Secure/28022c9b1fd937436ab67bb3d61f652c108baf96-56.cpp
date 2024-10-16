static void php_image_filter_contrast(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *SIM;
	gdImagePtr im_src;
	zend_long contrast, tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll", &SIM, &tmp, &contrast) == FAILURE) {
		RETURN_FALSE;
	}

	if ((im_src = (gdImagePtr)zend_fetch_resource(Z_RES_P(SIM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (im_src == NULL) {
		RETURN_FALSE;
	}

	if (gdImageContrast(im_src, (int)contrast) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}