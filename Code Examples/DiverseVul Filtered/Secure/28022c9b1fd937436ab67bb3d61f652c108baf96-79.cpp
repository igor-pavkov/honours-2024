PHP_FUNCTION(imagedashedline)
{
	zval *IM;
	zend_long x1, y1, x2, y2, col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlllll", &IM, &x1, &y1, &x2, &y2, &col) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	gdImageDashedLine(im, x1, y1, x2, y2, col);
	RETURN_TRUE;
}