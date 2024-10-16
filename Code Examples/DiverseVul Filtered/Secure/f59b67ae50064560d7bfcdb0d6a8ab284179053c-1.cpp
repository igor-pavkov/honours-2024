static inline php_stream *phar_get_pharufp(phar_archive_data *phar TSRMLS_DC)
{
	if (!phar->is_persistent) {
		return phar->ufp;
	}
	return PHAR_GLOBALS->cached_fp[phar->phar_pos].ufp;
}