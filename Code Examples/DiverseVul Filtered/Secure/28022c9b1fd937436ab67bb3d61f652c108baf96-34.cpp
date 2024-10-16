static void php_imagefontsize(INTERNAL_FUNCTION_PARAMETERS, int arg)
{
	zend_long SIZE;
	gdFontPtr font;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &SIZE) == FAILURE) {
		return;
	}

	font = php_find_gd_font(SIZE);
	RETURN_LONG(arg ? font->h : font->w);
}