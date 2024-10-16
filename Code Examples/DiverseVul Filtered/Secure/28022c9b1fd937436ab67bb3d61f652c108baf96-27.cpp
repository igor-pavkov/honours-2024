PHP_FUNCTION(imageellipse)
{
	zval *IM;
	zend_long cx, cy, w, h, color;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlllll", &IM, &cx, &cy, &w, &h, &color) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	gdImageEllipse(im, cx, cy, w, h, color);
	RETURN_TRUE;
}