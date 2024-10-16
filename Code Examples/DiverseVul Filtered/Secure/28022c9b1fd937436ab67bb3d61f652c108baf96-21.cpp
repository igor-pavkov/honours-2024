PHP_FUNCTION(imagecrop)
{
	zval *IM;
	gdImagePtr im;
	gdImagePtr im_crop;
	gdRect rect;
	zval *z_rect;
	zval *tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ra", &IM, &z_rect) == FAILURE)  {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "x", sizeof("x") -1)) != NULL) {
		rect.x = zval_get_long(tmp);
	} else {
		php_error_docref(NULL, E_WARNING, "Missing x position");
		RETURN_FALSE;
	}

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "y", sizeof("y") - 1)) != NULL) {
		rect.y = zval_get_long(tmp);
	} else {
		php_error_docref(NULL, E_WARNING, "Missing y position");
		RETURN_FALSE;
	}

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "width", sizeof("width") - 1)) != NULL) {
		rect.width = zval_get_long(tmp);
	} else {
		php_error_docref(NULL, E_WARNING, "Missing width");
		RETURN_FALSE;
	}

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "height", sizeof("height") - 1)) != NULL) {
		rect.height = zval_get_long(tmp);
	} else {
		php_error_docref(NULL, E_WARNING, "Missing height");
		RETURN_FALSE;
	}

	im_crop = gdImageCrop(im, &rect);

	if (im_crop == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_RES(zend_register_resource(im_crop, le_gd));
	}
}