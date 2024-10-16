PHP_FUNCTION(imagecolorresolvealpha)
{
	zval *IM;
	zend_long red, green, blue, alpha;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rllll", &IM, &red, &green, &blue, &alpha) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(gdImageColorResolveAlpha(im, red, green, blue, alpha));
}