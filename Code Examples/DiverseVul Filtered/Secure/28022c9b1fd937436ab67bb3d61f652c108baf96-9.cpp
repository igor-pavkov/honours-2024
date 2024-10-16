static void php_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int mode, int extended)
{
	zval *IM, *EXT = NULL;
	gdImagePtr im=NULL;
	zend_long col = -1, x = -1, y = -1;
	size_t str_len, fontname_len;
	int i, brect[8];
	double ptsize, angle;
	char *str = NULL, *fontname = NULL;
	char *error = NULL;
	int argc = ZEND_NUM_ARGS();
	gdFTStringExtra strex = {0};

	if (mode == TTFTEXT_BBOX) {
		if (argc < 4 || argc > ((extended) ? 5 : 4)) {
			ZEND_WRONG_PARAM_COUNT();
		} else if (zend_parse_parameters(argc, "ddss|a", &ptsize, &angle, &fontname, &fontname_len, &str, &str_len, &EXT) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (argc < 8 || argc > ((extended) ? 9 : 8)) {
			ZEND_WRONG_PARAM_COUNT();
		} else if (zend_parse_parameters(argc, "rddlllss|a", &IM, &ptsize, &angle, &x, &y, &col, &fontname, &fontname_len, &str, &str_len, &EXT) == FAILURE) {
			RETURN_FALSE;
		}
		if ((im = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
			RETURN_FALSE;
		}
	}

	/* convert angle to radians */
	angle = angle * (M_PI/180);

	if (extended && EXT) {	/* parse extended info */
		zval *item;
		zend_string *key;

		/* walk the assoc array */
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(EXT), key, item) {
			if (key == NULL) {
				continue;
			}
			if (strcmp("linespacing", ZSTR_VAL(key)) == 0) {
				strex.flags |= gdFTEX_LINESPACE;
				strex.linespacing = zval_get_double(item);
			}
		} ZEND_HASH_FOREACH_END();
	}

#ifdef VIRTUAL_DIR
	{
		char tmp_font_path[MAXPATHLEN];

		if (!VCWD_REALPATH(fontname, tmp_font_path)) {
			fontname = NULL;
		}
	}
#endif /* VIRTUAL_DIR */

	PHP_GD_CHECK_OPEN_BASEDIR(fontname, "Invalid font filename");

#ifdef HAVE_GD_FREETYPE
	if (extended) {
		error = gdImageStringFTEx(im, brect, col, fontname, ptsize, angle, x, y, str, &strex);
	}
	else
		error = gdImageStringFT(im, brect, col, fontname, ptsize, angle, x, y, str);

#endif /* HAVE_GD_FREETYPE */

	if (error) {
		php_error_docref(NULL, E_WARNING, "%s", error);
		RETURN_FALSE;
	}

	array_init(return_value);

	/* return array with the text's bounding box */
	for (i = 0; i < 8; i++) {
		add_next_index_long(return_value, brect[i]);
	}
}