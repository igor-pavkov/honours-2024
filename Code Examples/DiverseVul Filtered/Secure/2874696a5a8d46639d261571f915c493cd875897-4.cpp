inline static int php_openssl_open_base_dir_chk(char *filename TSRMLS_DC)
{
	if (php_check_open_basedir(filename TSRMLS_CC)) {
		return -1;
	}

	return 0;
}