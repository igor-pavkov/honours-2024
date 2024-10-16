static char **make_subpats_table(int num_subpats, pcre_cache_entry *pce TSRMLS_DC)
{
	pcre_extra *extra = pce->extra;
	int name_cnt = 0, name_size, ni = 0;
	int rc;
	char *name_table;
	unsigned short name_idx;
	char **subpat_names = (char **)ecalloc(num_subpats, sizeof(char *));

	rc = pcre_fullinfo(pce->re, extra, PCRE_INFO_NAMECOUNT, &name_cnt);
	if (rc < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		efree(subpat_names);
		return NULL;
	}
	if (name_cnt > 0) {
		int rc1, rc2;

		rc1 = pcre_fullinfo(pce->re, extra, PCRE_INFO_NAMETABLE, &name_table);
		rc2 = pcre_fullinfo(pce->re, extra, PCRE_INFO_NAMEENTRYSIZE, &name_size);
		rc = rc2 ? rc2 : rc1;
		if (rc < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
			efree(subpat_names);
			return NULL;
		}

		while (ni++ < name_cnt) {
			name_idx = 0xff * (unsigned char)name_table[0] + (unsigned char)name_table[1];
			subpat_names[name_idx] = name_table + 2;
			if (is_numeric_string(subpat_names[name_idx], strlen(subpat_names[name_idx]), NULL, NULL, 0) > 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Numeric named subpatterns are not allowed");
				efree(subpat_names);
				return NULL;
			}
			name_table += name_size;
		}
	}

	return subpat_names;
}