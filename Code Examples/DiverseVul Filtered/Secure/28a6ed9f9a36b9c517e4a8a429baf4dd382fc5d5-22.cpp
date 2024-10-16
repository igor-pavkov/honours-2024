static int spl_dllist_object_count_elements(zval *object, zend_long *count) /* {{{ */
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(object);

	if (intern->fptr_count) {
		zval rv;
		zend_call_method_with_0_params(object, intern->std.ce, &intern->fptr_count, "count", &rv);
		if (!Z_ISUNDEF(rv)) {
			*count = zval_get_long(&rv);
			zval_ptr_dtor(&rv);
			return SUCCESS;
		}
		*count = 0;
		return FAILURE;
	}

	*count = spl_ptr_llist_count(intern->llist);
	return SUCCESS;
}