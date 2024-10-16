SPL_METHOD(SplFileInfo, getFileInfo)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	zend_class_entry *ce = intern->info_class;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|C", &ce) == SUCCESS) {
		spl_filesystem_object_create_type(ZEND_NUM_ARGS(), intern, SPL_FS_INFO, ce, return_value);
	}

	zend_restore_error_handling(&error_handling);
}