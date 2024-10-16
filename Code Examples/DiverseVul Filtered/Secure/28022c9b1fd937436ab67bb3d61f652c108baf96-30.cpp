PHP_FUNCTION(imagepalettecopy)
{
	zval *dstim, *srcim;
	gdImagePtr dst, src;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &dstim, &srcim) == FAILURE) {
		return;
	}

	if ((dst = (gdImagePtr)zend_fetch_resource(Z_RES_P(dstim), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if ((src = (gdImagePtr)zend_fetch_resource(Z_RES_P(srcim), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	gdImagePaletteCopy(dst, src);
}