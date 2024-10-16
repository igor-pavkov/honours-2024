void *zend_shared_alloc_get_xlat_entry(const void *old)
{
	void *retval;

	if ((retval = zend_hash_index_find_ptr(&xlat_table, (zend_ulong)old)) == NULL) {
		return NULL;
	}
	return retval;
}