static inline void phar_set_pharfp(phar_archive_data *phar, php_stream *fp TSRMLS_DC)
{
	if (!phar->is_persistent) {
		phar->fp =  fp;
		return;
	}

	PHAR_GLOBALS->cached_fp[phar->phar_pos].fp = fp;
}