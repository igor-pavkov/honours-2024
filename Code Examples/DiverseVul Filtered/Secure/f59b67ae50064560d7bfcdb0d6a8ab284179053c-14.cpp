static inline php_stream *phar_get_entrypufp(phar_entry_info *entry TSRMLS_DC)
{
	if (!entry->is_persistent) {
		return entry->phar->ufp;
	}
	return PHAR_GLOBALS->cached_fp[entry->phar->phar_pos].ufp;
}