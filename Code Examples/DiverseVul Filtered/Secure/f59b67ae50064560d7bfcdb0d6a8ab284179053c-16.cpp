static inline void phar_set_pharufp(phar_archive_data *phar, php_stream *fp TSRMLS_DC)
{
	if (!phar->is_persistent) {
		phar->ufp =  fp;
		return;
	}

	PHAR_GLOBALS->cached_fp[phar->phar_pos].ufp = fp;
}