static int extract_helper(phar_archive_data *archive, zend_string *search, char *pathto, size_t pathto_len, zend_bool overwrite, char **error) { /* {{{ */
	int extracted = 0;
	phar_entry_info *entry;

	if (!search) {
		/* nothing to match -- extract all files */
		ZEND_HASH_FOREACH_PTR(&archive->manifest, entry) {
			if (FAILURE == phar_extract_file(overwrite, entry, pathto, (int)pathto_len, error)) return -1;
			extracted++;
		} ZEND_HASH_FOREACH_END();
	} else if ('/' == ZSTR_VAL(search)[ZSTR_LEN(search) - 1]) {
		/* ends in "/" -- extract all entries having that prefix */
		ZEND_HASH_FOREACH_PTR(&archive->manifest, entry) {
			if (0 != strncmp(ZSTR_VAL(search), entry->filename, ZSTR_LEN(search))) continue;
			if (FAILURE == phar_extract_file(overwrite, entry, pathto, (int)pathto_len, error)) return -1;
			extracted++;
		} ZEND_HASH_FOREACH_END();
	} else {
		/* otherwise, looking for an exact match */
		entry = zend_hash_find_ptr(&archive->manifest, search);
		if (NULL == entry) return 0;
		if (FAILURE == phar_extract_file(overwrite, entry, pathto, (int)pathto_len, error)) return -1;
		return 1;
	}

	return extracted;
}