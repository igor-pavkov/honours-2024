PHP_FUNCTION(imagesetinterpolation)
{
	zval *IM;
	gdImagePtr im;
	zend_long method = GD_BILINEAR_FIXED;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &IM, &method) == FAILURE)  {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (method == -1) {
		 method = GD_BILINEAR_FIXED;
	}
	RETURN_BOOL(gdImageSetInterpolationMethod(im, (gdInterpolationMethod) method));
}