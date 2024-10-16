PHP_METHOD(Phar, __destruct)
{
	phar_archive_object *phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (phar_obj->arc.archive && phar_obj->arc.archive->is_persistent) {
		zend_hash_del(&PHAR_GLOBALS->phar_persist_map, (const char *) phar_obj->arc.archive, sizeof(phar_obj->arc.archive));
	}
}