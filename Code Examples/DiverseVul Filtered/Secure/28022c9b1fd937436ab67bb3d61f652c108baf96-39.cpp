PHP_FUNCTION(imagedestroy)
{
	zval *IM;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &IM) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	zend_list_close(Z_RES_P(IM));

	RETURN_TRUE;
}