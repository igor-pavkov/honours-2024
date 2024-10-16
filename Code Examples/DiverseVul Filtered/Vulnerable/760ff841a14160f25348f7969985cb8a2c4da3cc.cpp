ZEND_API int ZEND_FASTCALL concat_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
    zval *orig_op1 = op1;
	zval op1_copy, op2_copy;

	ZVAL_UNDEF(&op1_copy);
	ZVAL_UNDEF(&op2_copy);

	do {
	 	if (UNEXPECTED(Z_TYPE_P(op1) != IS_STRING)) {
	 		if (Z_ISREF_P(op1)) {
	 			op1 = Z_REFVAL_P(op1);
	 			if (Z_TYPE_P(op1) == IS_STRING) break;
	 		}
			ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_CONCAT, concat_function);
			ZVAL_STR(&op1_copy, zval_get_string_func(op1));
			if (UNEXPECTED(EG(exception))) {
				zval_ptr_dtor_str(&op1_copy);
				if (orig_op1 != result) {
					ZVAL_UNDEF(result);
				}
				return FAILURE;
			}
			if (result == op1) {
				if (UNEXPECTED(op1 == op2)) {
					op2 = &op1_copy;
				}
			}
			op1 = &op1_copy;
		}
	} while (0);
	do {
		if (UNEXPECTED(Z_TYPE_P(op2) != IS_STRING)) {
	 		if (Z_ISREF_P(op2)) {
	 			op2 = Z_REFVAL_P(op2);
	 			if (Z_TYPE_P(op2) == IS_STRING) break;
	 		}
			ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(ZEND_CONCAT);
			ZVAL_STR(&op2_copy, zval_get_string_func(op2));
			if (UNEXPECTED(EG(exception))) {
				zval_ptr_dtor_str(&op1_copy);
				zval_ptr_dtor_str(&op2_copy);
				if (orig_op1 != result) {
					ZVAL_UNDEF(result);
				}
				return FAILURE;
			}
			op2 = &op2_copy;
		}
	} while (0);

	if (UNEXPECTED(Z_STRLEN_P(op1) == 0)) {
		if (EXPECTED(result != op2)) {
			if (result == orig_op1) {
				i_zval_ptr_dtor(result);
			}
			ZVAL_COPY(result, op2);
		}
	} else if (UNEXPECTED(Z_STRLEN_P(op2) == 0)) {
		if (EXPECTED(result != op1)) {
			if (result == orig_op1) {
				i_zval_ptr_dtor(result);
			}
			ZVAL_COPY(result, op1);
		}
	} else {
		size_t op1_len = Z_STRLEN_P(op1);
		size_t op2_len = Z_STRLEN_P(op2);
		size_t result_len = op1_len + op2_len;
		zend_string *result_str;

		if (UNEXPECTED(op1_len > SIZE_MAX - op2_len)) {
			zend_throw_error(NULL, "String size overflow");
			zval_ptr_dtor_str(&op1_copy);
			zval_ptr_dtor_str(&op2_copy);
			if (orig_op1 != result) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}

		if (result == op1 && Z_REFCOUNTED_P(result)) {
			/* special case, perform operations on result */
			result_str = zend_string_extend(Z_STR_P(result), result_len, 0);
		} else {
			result_str = zend_string_alloc(result_len, 0);
			memcpy(ZSTR_VAL(result_str), Z_STRVAL_P(op1), op1_len);
			if (result == orig_op1) {
				i_zval_ptr_dtor(result);
			}
		}

		/* This has to happen first to account for the cases where result == op1 == op2 and
		 * the realloc is done. In this case this line will also update Z_STRVAL_P(op2) to
		 * point to the new string. The first op2_len bytes of result will still be the same. */
		ZVAL_NEW_STR(result, result_str);

		memcpy(ZSTR_VAL(result_str) + op1_len, Z_STRVAL_P(op2), op2_len);
		ZSTR_VAL(result_str)[result_len] = '\0';
	}

	zval_ptr_dtor_str(&op1_copy);
	zval_ptr_dtor_str(&op2_copy);
	return SUCCESS;
}