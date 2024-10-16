SPL_METHOD(SplFileInfo, setFileClass)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	zend_class_entry *ce = spl_ce_SplFileObject;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|C", &ce) == SUCCESS) {
		intern->file_class = ce;
	}

	zend_restore_error_handling(&error_handling);
}