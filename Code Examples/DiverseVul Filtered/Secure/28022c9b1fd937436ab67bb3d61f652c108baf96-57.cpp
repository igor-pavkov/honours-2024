PHP_FUNCTION(imageantialias)
{
	zval *IM;
	zend_bool alias;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rb", &IM, &alias) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}
	gdImageAntialias(im, alias);
	RETURN_TRUE;
}