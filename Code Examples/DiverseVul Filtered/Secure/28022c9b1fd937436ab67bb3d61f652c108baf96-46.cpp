static void php_image_filter_brightness(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *SIM;
	gdImagePtr im_src;
	zend_long brightness, tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zll", &SIM, &tmp, &brightness) == FAILURE) {
		RETURN_FALSE;
	}

	if ((im_src = (gdImagePtr)zend_fetch_resource(Z_RES_P(SIM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (im_src == NULL) {
		RETURN_FALSE;
	}

	if (gdImageBrightness(im_src, (int)brightness) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}