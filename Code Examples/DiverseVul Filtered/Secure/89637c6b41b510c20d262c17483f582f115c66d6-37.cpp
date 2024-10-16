static int php_openssl_write_rand_file(const char * file, int egdsocket, int seeded) /* {{{ */
{
	char buffer[MAXPATHLEN];

	TSRMLS_FETCH();

	if (egdsocket || !seeded) {
		/* if we did not manage to read the seed file, we should not write
		 * a low-entropy seed file back */
		return FAILURE;
	}
	if (file == NULL) {
		file = RAND_file_name(buffer, sizeof(buffer));
	}
	PHP_OPENSSL_RAND_ADD_TIME();
	if (file == NULL || !RAND_write_file(file)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to write random state");
		return FAILURE;
	}
	return SUCCESS;
}