void spl_array_iterator_key(zval *object, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	char *string_key;
	uint string_length;
	ulong num_key;
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (spl_array_object_verify_pos(intern, aht TSRMLS_CC) == FAILURE) {
		return;
	}

	switch (zend_hash_get_current_key_ex(aht, &string_key, &string_length, &num_key, 1, &intern->pos)) {
		case HASH_KEY_IS_STRING:
			RETVAL_STRINGL(string_key, string_length - 1, 0);
			break;
		case HASH_KEY_IS_LONG:
			RETVAL_LONG(num_key);
			break;
		case HASH_KEY_NON_EXISTANT:
			return;
	}
}