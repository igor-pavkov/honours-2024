static inline void phar_set_entrypufp(phar_entry_info *entry, php_stream *fp TSRMLS_DC)
{
	if (!entry->phar->is_persistent) {
		entry->phar->ufp =  fp;
		return;
	}

	PHAR_GLOBALS->cached_fp[entry->phar->phar_pos].ufp = fp;
}