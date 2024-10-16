static PHP_INI_MH(OnUpdateInputEncoding)
{
	if (ZSTR_LEN(new_value) >= ICONV_CSNMAXLEN) {
		return FAILURE;
	}
	if (stage & (PHP_INI_STAGE_ACTIVATE | PHP_INI_STAGE_RUNTIME)) {
		php_error_docref("ref.iconv", E_DEPRECATED, "Use of iconv.input_encoding is deprecated");
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}