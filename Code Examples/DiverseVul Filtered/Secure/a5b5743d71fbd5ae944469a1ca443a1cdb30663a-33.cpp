static void php_filter_call(zval **filtered, long filter, zval **filter_args, const int copy, long filter_flags TSRMLS_DC) /* {{{ */
{
	zval  *options = NULL;
	zval **option;
	char  *charset = NULL;

	if (filter_args && Z_TYPE_PP(filter_args) != IS_ARRAY) {
		long lval;

		PHP_FILTER_GET_LONG_OPT(filter_args, lval);

		if (filter != -1) { /* handler for array apply */
			/* filter_args is the filter_flags */
			filter_flags = lval;

			if (!(filter_flags & FILTER_REQUIRE_ARRAY ||  filter_flags & FILTER_FORCE_ARRAY)) {
				filter_flags |= FILTER_REQUIRE_SCALAR;
			}
		} else {
			filter = lval;
		}
	} else if (filter_args) {
		if (zend_hash_find(HASH_OF(*filter_args), "filter", sizeof("filter"), (void **)&option) == SUCCESS) {
			PHP_FILTER_GET_LONG_OPT(option, filter);
		}

		if (zend_hash_find(HASH_OF(*filter_args), "flags", sizeof("flags"), (void **)&option) == SUCCESS) {
			PHP_FILTER_GET_LONG_OPT(option, filter_flags);

			if (!(filter_flags & FILTER_REQUIRE_ARRAY ||  filter_flags & FILTER_FORCE_ARRAY)) {
				filter_flags |= FILTER_REQUIRE_SCALAR;
			}
		}

		if (zend_hash_find(HASH_OF(*filter_args), "options", sizeof("options"), (void **)&option) == SUCCESS) {
			if (filter != FILTER_CALLBACK) {
				if (Z_TYPE_PP(option) == IS_ARRAY) {
					options = *option;
				}
			} else {
				options = *option;
				filter_flags = 0;
			}
		}
	}

	if (Z_TYPE_PP(filtered) == IS_ARRAY) {
		if (filter_flags & FILTER_REQUIRE_SCALAR) {
			if (copy) {
				SEPARATE_ZVAL(filtered);
			}
			zval_dtor(*filtered);
			if (filter_flags & FILTER_NULL_ON_FAILURE) {
				ZVAL_NULL(*filtered);
			} else {
				ZVAL_FALSE(*filtered);
			}
			return;
		}
		php_zval_filter_recursive(filtered, filter, filter_flags, options, charset, copy TSRMLS_CC);
		return;
	}
	if (filter_flags & FILTER_REQUIRE_ARRAY) {
		if (copy) {
			SEPARATE_ZVAL(filtered);
		}
		zval_dtor(*filtered);
		if (filter_flags & FILTER_NULL_ON_FAILURE) {
			ZVAL_NULL(*filtered);
		} else {
			ZVAL_FALSE(*filtered);
		}
		return;
	}

	php_zval_filter(filtered, filter, filter_flags, options, charset, copy TSRMLS_CC);
	if (filter_flags & FILTER_FORCE_ARRAY) {
		zval *tmp;

		ALLOC_ZVAL(tmp);
		MAKE_COPY_ZVAL(filtered, tmp);

		zval_dtor(*filtered);

		array_init(*filtered);
		add_next_index_zval(*filtered, tmp);
	}
}