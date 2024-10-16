PHP_FUNCTION(imagerotate)
{
	zval *SIM;
	gdImagePtr im_dst, im_src;
	double degrees;
	zend_long color;
	zend_long ignoretransparent = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rdl|l", &SIM, &degrees, &color, &ignoretransparent) == FAILURE) {
		RETURN_FALSE;
	}

	if ((im_src = (gdImagePtr)zend_fetch_resource(Z_RES_P(SIM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	im_dst = gdImageRotateInterpolated(im_src, (const float)degrees, color);

	if (im_dst != NULL) {
		RETURN_RES(zend_register_resource(im_dst, le_gd));
	} else {
		RETURN_FALSE;
	}
}