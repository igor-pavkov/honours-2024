PHP_FUNCTION(imagesetbrush)
{
	zval *IM, *TILE;
	gdImagePtr im, tile;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &IM, &TILE) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if ((tile = (gdImagePtr)zend_fetch_resource(Z_RES_P(TILE), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	gdImageSetBrush(im, tile);

	RETURN_TRUE;
}