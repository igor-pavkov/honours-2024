void zend_shared_alloc_register_xlat_entry(const void *old, const void *new)
{
	zend_hash_index_update(&xlat_table, (ulong)old, (void*)&new, sizeof(void *), NULL);
}