static inline void phar_set_fp_type(phar_entry_info *entry, enum phar_fp_type type, off_t offset TSRMLS_DC)
{
	phar_entry_fp_info *data;

	if (!entry->is_persistent) {
		entry->fp_type = type;
		entry->offset = offset;
		return;
	}
	data = &(PHAR_GLOBALS->cached_fp[entry->phar->phar_pos].manifest[entry->manifest_pos]);
	data->fp_type = type;
	data->offset = offset;
}