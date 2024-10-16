PHP_FUNCTION(imagescale)
{
	zval *IM;
	gdImagePtr im;
	gdImagePtr im_scaled = NULL;
	int new_width, new_height;
	zend_long tmp_w, tmp_h=-1, tmp_m = GD_BILINEAR_FIXED;
	gdInterpolationMethod method;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl|ll", &IM, &tmp_w, &tmp_h, &tmp_m) == FAILURE)  {
		return;
	}
	method = tmp_m;

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (tmp_h < 0) {
		/* preserve ratio */
		long src_x, src_y;

		src_x = gdImageSX(im);
		src_y = gdImageSY(im);
		if (src_x) {
			tmp_h = tmp_w * src_y / src_x;
		}
	}

	if (tmp_h <= 0 || tmp_w <= 0) {
		RETURN_FALSE;
	}

	new_width = tmp_w;
	new_height = tmp_h;

	if (gdImageSetInterpolationMethod(im, method)) {
		im_scaled = gdImageScale(im, new_width, new_height);
	}

	if (im_scaled == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_RES(zend_register_resource(im_scaled, le_gd));
	}
}