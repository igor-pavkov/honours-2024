SPL_METHOD(SplFileObject, fpassthru)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	RETURN_LONG(php_stream_passthru(intern->u.file.stream));
} /* }}} */