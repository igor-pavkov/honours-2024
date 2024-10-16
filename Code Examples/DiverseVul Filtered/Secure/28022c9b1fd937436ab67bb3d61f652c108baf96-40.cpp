PHP_FUNCTION(imagearc)
{
	zval *IM;
	zend_long cx, cy, w, h, ST, E, col;
	gdImagePtr im;
	int e, st;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlllllll", &IM, &cx, &cy, &w, &h, &ST, &E, &col) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	e = E;
	if (e < 0) {
		e %= 360;
	}

	st = ST;
	if (st < 0) {
		st %= 360;
	}

	gdImageArc(im, cx, cy, w, h, st, e, col);
	RETURN_TRUE;
}