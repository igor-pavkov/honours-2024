static void phar_postprocess_ru_web(char *fname, int fname_len, char **entry, int *entry_len, char **ru, int *ru_len) /* {{{ */
{
	char *e = *entry + 1, *u = NULL, *u1 = NULL, *saveu = NULL;
	int e_len = *entry_len - 1, u_len = 0;
	phar_archive_data *pphar;

	/* we already know we can retrieve the phar if we reach here */
	pphar = zend_hash_str_find_ptr(&(PHAR_G(phar_fname_map)), fname, fname_len);

	if (!pphar && PHAR_G(manifest_cached)) {
		pphar = zend_hash_str_find_ptr(&cached_phars, fname, fname_len);
	}

	do {
		if (zend_hash_str_exists(&(pphar->manifest), e, e_len)) {
			if (u) {
				u[0] = '/';
				*ru = estrndup(u, u_len+1);
				++u_len;
				u[0] = '\0';
			} else {
				*ru = NULL;
			}
			*ru_len = u_len;
			*entry_len = e_len + 1;
			return;
		}

		if (u) {
			u1 = strrchr(e, '/');
			u[0] = '/';
			saveu = u;
			e_len += u_len + 1;
			u = u1;
			if (!u) {
				return;
			}
		} else {
			u = strrchr(e, '/');
			if (!u) {
				if (saveu) {
					saveu[0] = '/';
				}
				return;
			}
		}

		u[0] = '\0';
		u_len = (int)strlen(u + 1);
		e_len -= u_len + 1;

		if (e_len < 0) {
			if (saveu) {
				saveu[0] = '/';
			}
			return;
		}
	} while (1);
}