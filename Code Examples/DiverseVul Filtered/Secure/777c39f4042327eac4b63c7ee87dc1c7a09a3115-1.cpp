size_t zend_shared_alloc_get_free_memory(void)
{
	return ZSMMG(shared_free);
}