PHP_FUNCTION(imagecopyresized)
{
	zval *SIM, *DIM;
	zend_long SX, SY, SW, SH, DX, DY, DW, DH;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, dstH, dstW, srcY, srcX, dstY, dstX;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rrllllllll", &DIM, &SIM, &DX, &DY, &SX, &SY, &DW, &DH, &SW, &SH) == FAILURE) {
		return;
	}

	if ((im_dst = (gdImagePtr)zend_fetch_resource(Z_RES_P(DIM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if ((im_src = (gdImagePtr)zend_fetch_resource(Z_RES_P(SIM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	srcX = SX;
	srcY = SY;
	srcH = SH;
	srcW = SW;
	dstX = DX;
	dstY = DY;
	dstH = DH;
	dstW = DW;

	if (dstW <= 0 || dstH <= 0 || srcW <= 0 || srcH <= 0) {
		php_error_docref(NULL, E_WARNING, "Invalid image dimensions");
		RETURN_FALSE;
	}

	gdImageCopyResized(im_dst, im_src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
	RETURN_TRUE;
}