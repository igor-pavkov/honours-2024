PHP_FUNCTION(imagelayereffect)
{
	zval *IM;
	zend_long effect;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &IM, &effect) == FAILURE) {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	gdImageAlphaBlending(im, effect);

	RETURN_TRUE;
}