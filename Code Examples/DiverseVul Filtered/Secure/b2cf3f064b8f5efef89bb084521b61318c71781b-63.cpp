static void phar_mkdir(phar_archive_data **pphar, char *dirname, int dirname_len TSRMLS_DC)
{
	char *error;
	phar_entry_data *data;

	if (!(data = phar_get_or_create_entry_data((*pphar)->fname, (*pphar)->fname_len, dirname, dirname_len, "w+b", 2, &error, 1 TSRMLS_CC))) {
		if (error) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Directory %s does not exist and cannot be created: %s", dirname, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Directory %s does not exist and cannot be created", dirname);
		}

		return;
	} else {
		if (error) {
			efree(error);
		}

		/* check for copy on write */
		if (data->phar != *pphar) {
			*pphar = data->phar;
		}
		phar_entry_delref(data TSRMLS_CC);
		phar_flush(*pphar, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
	}
}