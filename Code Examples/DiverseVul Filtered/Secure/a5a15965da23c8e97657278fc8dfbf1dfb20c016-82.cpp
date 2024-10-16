SPL_METHOD(DirectoryIterator, current)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	ZVAL_OBJ(return_value, Z_OBJ_P(getThis()));
	Z_ADDREF_P(return_value);
}