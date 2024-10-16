PHP_FUNCTION(imagecopymergegray)
{
	zval *SIM, *DIM;
	zend_long SX, SY, SW, SH, DX, DY, PCT;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX, pct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rrlllllll", &DIM, &SIM, &DX, &DY, &SX, &SY, &SW, &SH, &PCT) == FAILURE) {
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
	pct  = PCT;

	gdImageCopyMergeGray(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH, pct);
	RETURN_TRUE;
}