SPL_METHOD(SplFileObject, fscanf)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	spl_filesystem_file_free_line(intern);
	intern->u.file.current_line_num++;

	FileFunctionCall(fscanf, ZEND_NUM_ARGS(), NULL);
}