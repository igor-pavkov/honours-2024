inline static int php_openssl_open_base_dir_chk(char *filename)
{
	if (php_check_open_basedir(filename)) {
		return -1;
	}

	return 0;
}