void zend_shared_alloc_register_xlat_entry(const void *old, const void *new)
{
	zend_hash_index_update_ptr(&xlat_table, (zend_ulong)old, (void*)new);
}