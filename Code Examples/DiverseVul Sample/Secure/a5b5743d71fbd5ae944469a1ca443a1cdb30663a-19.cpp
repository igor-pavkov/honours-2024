static void php_zval_filter(zval **value, long filter, long flags, zval *options, char* charset, zend_bool copy TSRMLS_DC) /* {{{ */
{
	filter_list_entry  filter_func;

	filter_func = php_find_filter(filter);

	if (!filter_func.id) {
		/* Find default filter */
		filter_func = php_find_filter(FILTER_DEFAULT);
	}

	if (copy) {
		SEPARATE_ZVAL(value);
	}

	/* #49274, fatal error with object without a toString method
	  Fails nicely instead of getting a recovarable fatal error. */
	if (Z_TYPE_PP(value) == IS_OBJECT) {
		zend_class_entry *ce;

		ce = Z_OBJCE_PP(value);
		if (!ce->__tostring) {
			ZVAL_FALSE(*value);
			return;
		}
	}

	/* Here be strings */
	convert_to_string(*value);

	filter_func.function(*value, flags, options, charset TSRMLS_CC);

	if (
		options && (Z_TYPE_P(options) == IS_ARRAY || Z_TYPE_P(options) == IS_OBJECT) &&
		((flags & FILTER_NULL_ON_FAILURE && Z_TYPE_PP(value) == IS_NULL) || 
		(!(flags & FILTER_NULL_ON_FAILURE) && Z_TYPE_PP(value) == IS_BOOL && Z_LVAL_PP(value) == 0)) &&
		zend_hash_exists(HASH_OF(options), "default", sizeof("default"))
	) {
		zval **tmp; 
		if (zend_hash_find(HASH_OF(options), "default", sizeof("default"), (void **)&tmp) == SUCCESS) {
			MAKE_COPY_ZVAL(tmp, *value);
		}
	}
}