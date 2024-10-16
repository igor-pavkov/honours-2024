static int phar_set_compression(void *pDest, void *argument TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)pDest;
	php_uint32 compress = *(php_uint32 *)argument;

	if (entry->is_deleted) {
		return ZEND_HASH_APPLY_KEEP;
	}

	entry->old_flags = entry->flags;
	entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry->flags |= compress;
	entry->is_modified = 1;
	return ZEND_HASH_APPLY_KEEP;
}