PHP_FUNCTION(imagecreate)
{
	zend_long x_size, y_size;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &x_size, &y_size) == FAILURE) {
		return;
	}

	if (x_size <= 0 || y_size <= 0 || x_size >= INT_MAX || y_size >= INT_MAX) {
		php_error_docref(NULL, E_WARNING, "Invalid image dimensions");
		RETURN_FALSE;
	}

	im = gdImageCreate(x_size, y_size);

	if (!im) {
		RETURN_FALSE;
	}

	RETURN_RES(zend_register_resource(im, le_gd));
}