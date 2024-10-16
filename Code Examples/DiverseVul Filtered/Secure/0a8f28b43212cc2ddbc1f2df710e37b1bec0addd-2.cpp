int zend_shared_memdup_size(void *source, size_t size)
{
	void **old_p;

	if (zend_hash_index_find(&xlat_table, (ulong)source, (void **)&old_p) == SUCCESS) {
		/* we already duplicated this pointer */
		return 0;
	}
	zend_shared_alloc_register_xlat_entry(source, source);
	return ZEND_ALIGNED_SIZE(size);
}