PHP_FUNCTION(imageinterlace)
{
	zval *IM;
	int argc = ZEND_NUM_ARGS();
	zend_long INT = 0;
	gdImagePtr im;

	if (zend_parse_parameters(argc, "r|l", &IM, &INT) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if (argc > 1) {
		gdImageInterlace(im, INT);
	}

	RETURN_LONG(gdImageGetInterlaced(im));
}