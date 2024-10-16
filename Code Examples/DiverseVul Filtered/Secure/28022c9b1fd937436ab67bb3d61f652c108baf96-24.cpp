PHP_FUNCTION(imagealphablending)
{
	zval *IM;
	zend_bool blend;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rb", &IM, &blend) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	gdImageAlphaBlending(im, blend);

	RETURN_TRUE;
}