PHP_FUNCTION(imagecreatefromstring)
{
	zval *data;
	gdImagePtr im;
	int imtype;
	char sig[8];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &data) == FAILURE) {
		return;
	}

	convert_to_string_ex(data);
	if (Z_STRLEN_P(data) < 8) {
		php_error_docref(NULL, E_WARNING, "Empty string or invalid image");
		RETURN_FALSE;
	}

	memcpy(sig, Z_STRVAL_P(data), 8);

	imtype = _php_image_type(sig);

	switch (imtype) {
		case PHP_GDIMG_TYPE_JPG:
#ifdef HAVE_GD_JPG
			im = _php_image_create_from_string(data, "JPEG", gdImageCreateFromJpegCtx);
#else
			php_error_docref(NULL, E_WARNING, "No JPEG support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		case PHP_GDIMG_TYPE_PNG:
#ifdef HAVE_GD_PNG
			im = _php_image_create_from_string(data, "PNG", gdImageCreateFromPngCtx);
#else
			php_error_docref(NULL, E_WARNING, "No PNG support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		case PHP_GDIMG_TYPE_GIF:
			im = _php_image_create_from_string(data, "GIF", gdImageCreateFromGifCtx);
			break;

		case PHP_GDIMG_TYPE_WBM:
			im = _php_image_create_from_string(data, "WBMP", gdImageCreateFromWBMPCtx);
			break;

		case PHP_GDIMG_TYPE_GD2:
			im = _php_image_create_from_string(data, "GD2", gdImageCreateFromGd2Ctx);
			break;

		default:
			php_error_docref(NULL, E_WARNING, "Data is not in a recognized format");
			RETURN_FALSE;
	}

	if (!im) {
		php_error_docref(NULL, E_WARNING, "Couldn't create GD Image Stream out of Data");
		RETURN_FALSE;
	}

	RETURN_RES(zend_register_resource(im, le_gd));
}