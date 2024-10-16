PHP_FUNCTION(imagesetstyle)
{
	zval *IM, *styles, *item;
	gdImagePtr im;
	int *stylearr;
	int index = 0;
    uint32_t num_styles;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ra", &IM, &styles) == FAILURE)  {
		return;
	}

	if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

    num_styles = zend_hash_num_elements(Z_ARRVAL_P(styles));
    if (num_styles == 0) {
        php_error_docref(NULL, E_WARNING, "styles array must not be empty");
        RETURN_FALSE;
    }

	/* copy the style values in the stylearr */
	stylearr = safe_emalloc(sizeof(int), num_styles, 0);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(styles), item) {
		stylearr[index++] = zval_get_long(item);
	} ZEND_HASH_FOREACH_END();

	gdImageSetStyle(im, stylearr, index);

	efree(stylearr);

	RETURN_TRUE;
}