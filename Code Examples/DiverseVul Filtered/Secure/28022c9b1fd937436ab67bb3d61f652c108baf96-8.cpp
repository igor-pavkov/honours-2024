PHP_FUNCTION(imagecropauto)
{
	zval *IM;
	zend_long mode = -1;
	zend_long color = -1;
	double threshold = 0.5f;
	gdImagePtr im;
	gdImagePtr im_crop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|ldl", &IM, &mode, &threshold, &color) == FAILURE)  {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	switch (mode) {
		case -1:
			mode = GD_CROP_DEFAULT;
		case GD_CROP_DEFAULT:
		case GD_CROP_TRANSPARENT:
		case GD_CROP_BLACK:
		case GD_CROP_WHITE:
		case GD_CROP_SIDES:
			im_crop = gdImageCropAuto(im, mode);
			break;

		case GD_CROP_THRESHOLD:
			if (color < 0 || (!gdImageTrueColor(im) && color >= gdImageColorsTotal(im))) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Color argument missing with threshold mode");
				RETURN_FALSE;
			}
			im_crop = gdImageCropThreshold(im, color, (float) threshold);
			break;

		default:
			php_error_docref(NULL, E_WARNING, "Unknown crop mode");
			RETURN_FALSE;
	}
	if (im_crop == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_RES(zend_register_resource(im_crop, le_gd));
	}
}