PHP_FUNCTION(imagefilltoborder)
{
	zval *IM;
	zend_long x, y, border, col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rllll", &IM, &x, &y, &border, &col) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	gdImageFillToBorder(im, x, y, border, col);
	RETURN_TRUE;
}