SPL_METHOD(SplFileInfo, openFile)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	spl_filesystem_object_create_type(ZEND_NUM_ARGS(), intern, SPL_FS_FILE, NULL, return_value);
}