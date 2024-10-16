PHP_FUNCTION(imagesetthickness)
{
	zval *IM;
	zend_long thick;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &IM, &thick) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	gdImageSetThickness(im, thick);

	RETURN_TRUE;
}