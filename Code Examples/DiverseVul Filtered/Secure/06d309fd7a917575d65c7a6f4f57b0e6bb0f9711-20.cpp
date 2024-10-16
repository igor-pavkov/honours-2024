static php_iconv_err_t _php_iconv_appendc(smart_str *d, const char c, iconv_t cd)
{
	return _php_iconv_appendl(d, &c, 1, cd);
}