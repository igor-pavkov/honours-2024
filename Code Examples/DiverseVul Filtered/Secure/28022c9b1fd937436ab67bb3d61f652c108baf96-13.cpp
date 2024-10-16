PHP_FUNCTION(imagecolordeallocate)
{
	zval *IM;
	zend_long index;
	int col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &IM, &index) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	/* We can return right away for a truecolor image as deallocating colours is meaningless here */
	if (gdImageTrueColor(im)) {
		RETURN_TRUE;
	}

	col = index;

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		gdImageColorDeallocate(im, col);
		RETURN_TRUE;
	} else {
		php_error_docref(NULL, E_WARNING, "Color index %d out of range",	col);
		RETURN_FALSE;
	}
}