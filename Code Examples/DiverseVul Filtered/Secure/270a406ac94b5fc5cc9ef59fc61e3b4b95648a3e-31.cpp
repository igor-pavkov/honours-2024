inline static int php_openssl_safe_mode_chk(char *filename TSRMLS_DC)
{
	if (PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		return -1;
	}
	if (php_check_open_basedir(filename TSRMLS_CC)) {
		return -1;
	}
	
	return 0;
}