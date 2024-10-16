static inline enum phar_fp_type phar_get_fp_type(phar_entry_info *entry TSRMLS_DC)
{
	if (!entry->is_persistent) {
		return entry->fp_type;
	}
	return PHAR_GLOBALS->cached_fp[entry->phar->phar_pos].manifest[entry->manifest_pos].fp_type;
}