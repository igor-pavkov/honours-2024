static void php_image_filter_colorize(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *SIM;
	gdImagePtr im_src;
	zend_long r,g,b,tmp;
	zend_long a = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rllll|l", &SIM, &tmp, &r, &g, &b, &a) == FAILURE) {
		RETURN_FALSE;
	}

	if ((im_src = (gdImagePtr)zend_fetch_resource(Z_RES_P(SIM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (im_src == NULL) {
		RETURN_FALSE;
	}

	if (gdImageColor(im_src, (int) r, (int) g, (int) b, (int) a) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}