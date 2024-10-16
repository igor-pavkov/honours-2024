PHP_FUNCTION(imagecolorclosest)
{
	zval *IM;
	zend_long red, green, blue;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlll", &IM, &red, &green, &blue) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(gdImageColorClosest(im, red, green, blue));
}