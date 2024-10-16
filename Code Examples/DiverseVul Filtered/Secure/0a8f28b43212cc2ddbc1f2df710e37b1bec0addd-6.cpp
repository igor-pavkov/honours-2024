void zend_shared_alloc_safe_unlock(TSRMLS_D)
{
	if (ZCG(locked)) {
		zend_shared_alloc_unlock(TSRMLS_C);
	}
}