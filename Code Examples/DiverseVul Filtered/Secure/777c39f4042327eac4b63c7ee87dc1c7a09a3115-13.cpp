void zend_shared_alloc_safe_unlock(void)
{
	if (ZCG(locked)) {
		zend_shared_alloc_unlock();
	}
}