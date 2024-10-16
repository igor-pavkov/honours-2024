ZEND_API const char *zend_get_executed_filename(void) /* {{{ */
{
	zend_execute_data *ex = EG(current_execute_data);

	while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->type))) {
		ex = ex->prev_execute_data;
	}
	if (ex) {
		return ZSTR_VAL(ex->func->op_array.filename);
	} else {
		return "[no active file]";
	}
}