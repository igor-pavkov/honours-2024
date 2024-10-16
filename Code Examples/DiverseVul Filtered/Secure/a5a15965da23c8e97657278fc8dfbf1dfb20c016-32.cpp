SPL_METHOD(SplFileInfo, setInfoClass)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	zend_class_entry *ce = spl_ce_SplFileInfo;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling );

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|C", &ce) == SUCCESS) {
		intern->info_class = ce;
	}

	zend_restore_error_handling(&error_handling);
}