PHP_FUNCTION(imageflip)
{
	zval *IM;
	zend_long mode;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &IM, &mode) == FAILURE)  {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	switch (mode) {
		case GD_FLIP_VERTICAL:
			gdImageFlipVertical(im);
			break;

		case GD_FLIP_HORINZONTAL:
			gdImageFlipHorizontal(im);
			break;

		case GD_FLIP_BOTH:
			gdImageFlipBoth(im);
			break;

		default:
			php_error_docref(NULL, E_WARNING, "Unknown flip mode");
			RETURN_FALSE;
	}

	RETURN_TRUE;
}