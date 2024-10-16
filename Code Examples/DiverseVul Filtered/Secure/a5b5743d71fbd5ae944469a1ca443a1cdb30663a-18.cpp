static void php_zval_filter_recursive(zval **value, long filter, long flags, zval *options, char *charset, zend_bool copy TSRMLS_DC) /* {{{ */
{
	if (Z_TYPE_PP(value) == IS_ARRAY) {
		zval **element;
		HashPosition pos;

		if (Z_ARRVAL_PP(value)->nApplyCount > 1) {
			return;
		}

		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(value), &pos);
			 zend_hash_get_current_data_ex(Z_ARRVAL_PP(value), (void **) &element, &pos) == SUCCESS;
			 zend_hash_move_forward_ex(Z_ARRVAL_PP(value), &pos)
		) {
			SEPARATE_ZVAL_IF_NOT_REF(element);
			if (Z_TYPE_PP(element) == IS_ARRAY) {
				Z_ARRVAL_PP(element)->nApplyCount++;
				php_zval_filter_recursive(element, filter, flags, options, charset, copy TSRMLS_CC);
				Z_ARRVAL_PP(element)->nApplyCount--;
			} else {
				php_zval_filter(element, filter, flags, options, charset, copy TSRMLS_CC);
			}
		}
	} else {
		php_zval_filter(value, filter, flags, options, charset, copy TSRMLS_CC);
	}
}