static inline php_stream *phar_get_entrypfp(phar_entry_info *entry TSRMLS_DC)
{
	if (!entry->is_persistent) {
		return entry->phar->fp;
	}
	return PHAR_GLOBALS->cached_fp[entry->phar->phar_pos].fp;
}