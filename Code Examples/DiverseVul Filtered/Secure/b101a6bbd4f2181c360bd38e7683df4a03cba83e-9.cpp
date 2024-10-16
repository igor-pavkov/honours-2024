int call_user_function(HashTable *function_table, zval *object, zval *function_name, zval *retval_ptr, uint32_t param_count, zval params[]) /* {{{ */
{
	return call_user_function_ex(function_table, object, function_name, retval_ptr, param_count, params, 1, NULL);
}