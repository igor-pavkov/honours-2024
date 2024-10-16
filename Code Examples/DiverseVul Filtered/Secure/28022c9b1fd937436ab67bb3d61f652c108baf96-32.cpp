PHP_FUNCTION(imagecolorsforindex)
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

	col = index;

	if ((col >= 0 && gdImageTrueColor(im)) || (!gdImageTrueColor(im) && col >= 0 && col < gdImageColorsTotal(im))) {
		array_init(return_value);

		add_assoc_long(return_value,"red",  gdImageRed(im,col));
		add_assoc_long(return_value,"green", gdImageGreen(im,col));
		add_assoc_long(return_value,"blue", gdImageBlue(im,col));
		add_assoc_long(return_value,"alpha", gdImageAlpha(im,col));
	} else {
		php_error_docref(NULL, E_WARNING, "Color index %d out of range", col);
		RETURN_FALSE;
	}
}