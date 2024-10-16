static void php_image_filter_smooth(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *SIM;
	zend_long tmp;
	gdImagePtr im_src;
	double weight;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rld", &SIM, &tmp, &weight) == FAILURE) {
		RETURN_FALSE;
	}

	if ((im_src = (gdImagePtr)zend_fetch_resource(Z_RES_P(SIM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (im_src == NULL) {
		RETURN_FALSE;
	}

	if (gdImageSmooth(im_src, (float)weight)==1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}