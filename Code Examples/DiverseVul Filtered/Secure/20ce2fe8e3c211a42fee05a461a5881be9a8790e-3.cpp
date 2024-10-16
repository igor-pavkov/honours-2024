PHPAPI void var_destroy(php_unserialize_data_t *var_hashx)
{
	void *next;
	zend_long i;
	var_entries *var_hash = (*var_hashx)->first;
	var_dtor_entries *var_dtor_hash = (*var_hashx)->first_dtor;
#if VAR_ENTRIES_DBG
	fprintf(stderr, "var_destroy(%ld)\n", var_hash?var_hash->used_slots:-1L);
#endif

	while (var_hash) {
		next = var_hash->next;
		efree_size(var_hash, sizeof(var_entries));
		var_hash = next;
	}

	while (var_dtor_hash) {
		for (i = 0; i < var_dtor_hash->used_slots; i++) {
#if VAR_ENTRIES_DBG
			fprintf(stderr, "var_destroy dtor(%p, %ld)\n", var_dtor_hash->data[i], Z_REFCOUNT_P(var_dtor_hash->data[i]));
#endif
			zval_ptr_dtor(&var_dtor_hash->data[i]);
		}
		next = var_dtor_hash->next;
		efree_size(var_dtor_hash, sizeof(var_dtor_entries));
		var_dtor_hash = next;
	}
}