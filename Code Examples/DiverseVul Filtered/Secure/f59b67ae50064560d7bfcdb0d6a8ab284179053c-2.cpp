static inline off_t phar_get_fp_offset(phar_entry_info *entry TSRMLS_DC)
{
	if (!entry->is_persistent) {
		return entry->offset;
	}
	if (PHAR_GLOBALS->cached_fp[entry->phar->phar_pos].manifest[entry->manifest_pos].fp_type == PHAR_FP) {
		if (!PHAR_GLOBALS->cached_fp[entry->phar->phar_pos].manifest[entry->manifest_pos].offset) {
			PHAR_GLOBALS->cached_fp[entry->phar->phar_pos].manifest[entry->manifest_pos].offset = entry->offset;
		}
	}
	return PHAR_GLOBALS->cached_fp[entry->phar->phar_pos].manifest[entry->manifest_pos].offset;
}