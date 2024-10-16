void *zend_shared_alloc_get_xlat_entry(const void *old)
{
	void **retval;

	if (zend_hash_index_find(&xlat_table, (ulong)old, (void **)&retval) == FAILURE) {
		return NULL;
	}
	return *retval;
}