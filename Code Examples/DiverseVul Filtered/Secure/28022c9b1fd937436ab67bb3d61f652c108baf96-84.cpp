PHP_FUNCTION(imagecolortransparent)
{
	zval *IM;
	zend_long COL = 0;
	gdImagePtr im;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "r|l", &IM, &COL) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (argc > 1) {
		gdImageColorTransparent(im, COL);
	}

	RETURN_LONG(gdImageGetTransparent(im));
}