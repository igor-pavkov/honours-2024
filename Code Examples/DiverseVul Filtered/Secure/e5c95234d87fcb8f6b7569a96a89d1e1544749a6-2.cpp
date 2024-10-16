PHP_METHOD(Phar, running)
{
	char *fname, *arch, *entry;
	int fname_len, arch_len, entry_len;
	zend_bool retphar = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &retphar) == FAILURE) {
		return;
	}

	fname = (char*)zend_get_executed_filename();
	fname_len = (int)strlen(fname);

	if (fname_len > 7 && !memcmp(fname, "phar://", 7) && SUCCESS == phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len, 2, 0)) {
		efree(entry);
		if (retphar) {
			RETVAL_STRINGL(fname, arch_len + 7);
			efree(arch);
			return;
		} else {
			// TODO: avoid reallocation ???
			RETVAL_STRINGL(arch, arch_len);
			efree(arch);
			return;
		}
	}

	RETURN_EMPTY_STRING();
}