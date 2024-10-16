static inline int object_common2(UNSERIALIZE_PARAMETER, zend_long elements)
{
	zval retval;
	zval fname;
	HashTable *ht;
	zend_bool has_wakeup;

	if (Z_TYPE_P(rval) != IS_OBJECT) {
		return 0;
	}

	has_wakeup = Z_OBJCE_P(rval) != PHP_IC_ENTRY
		&& zend_hash_str_exists(&Z_OBJCE_P(rval)->function_table, "__wakeup", sizeof("__wakeup")-1);

	ht = Z_OBJPROP_P(rval);
	zend_hash_extend(ht, zend_hash_num_elements(ht) + elements, (ht->u.flags & HASH_FLAG_PACKED));
	if (!process_nested_data(UNSERIALIZE_PASSTHRU, ht, elements, 1)) {
		if (has_wakeup) {
			ZVAL_DEREF(rval);
			GC_FLAGS(Z_OBJ_P(rval)) |= IS_OBJ_DESTRUCTOR_CALLED;
		}
		return 0;
	}

	ZVAL_DEREF(rval);
	if (has_wakeup) {
		ZVAL_STRINGL(&fname, "__wakeup", sizeof("__wakeup") - 1);
		BG(serialize_lock)++;
		if (call_user_function_ex(CG(function_table), rval, &fname, &retval, 0, 0, 1, NULL) == FAILURE || Z_ISUNDEF(retval)) {
			GC_FLAGS(Z_OBJ_P(rval)) |= IS_OBJ_DESTRUCTOR_CALLED;
		}
		BG(serialize_lock)--;
		zval_dtor(&fname);
		zval_dtor(&retval);
	}

	if (EG(exception)) {
		return 0;
	}

	return finish_nested_data(UNSERIALIZE_PASSTHRU);
}