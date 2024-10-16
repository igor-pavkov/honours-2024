PHP_FUNCTION(imagecolorset)
{
	zval *IM;
	zend_long color, red, green, blue, alpha = 0;
	int col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rllll|l", &IM, &color, &red, &green, &blue, &alpha) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	col = color;

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		im->red[col]   = red;
		im->green[col] = green;
		im->blue[col]  = blue;
		im->alpha[col]  = alpha;
	} else {
		RETURN_FALSE;
	}
}