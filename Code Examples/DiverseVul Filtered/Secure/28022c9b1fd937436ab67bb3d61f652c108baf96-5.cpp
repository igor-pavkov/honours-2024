PHP_FUNCTION(imagesavealpha)
{
	zval *IM;
	zend_bool save;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rb", &IM, &save) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	gdImageSaveAlpha(im, save);

	RETURN_TRUE;
}