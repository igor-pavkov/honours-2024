PHP_FUNCTION(imagecolorallocatealpha)
{
	zval *IM;
	zend_long red, green, blue, alpha;
	gdImagePtr im;
	int ct = (-1);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rllll", &IM, &red, &green, &blue, &alpha) == FAILURE) {
		RETURN_FALSE;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	ct = gdImageColorAllocateAlpha(im, red, green, blue, alpha);
	if (ct < 0) {
		RETURN_FALSE;
	}
	RETURN_LONG((zend_long)ct);
}