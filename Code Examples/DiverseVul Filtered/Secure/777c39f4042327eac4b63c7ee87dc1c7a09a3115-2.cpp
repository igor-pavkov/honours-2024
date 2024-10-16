void zend_shared_alloc_clear_xlat_table(void)
{
	zend_hash_clean(&xlat_table);
}