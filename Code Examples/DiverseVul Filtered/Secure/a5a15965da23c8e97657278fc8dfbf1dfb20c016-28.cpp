SPL_METHOD(SplFileObject, fgetcsv)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char delimiter = intern->u.file.delimiter, enclosure = intern->u.file.enclosure, escape = intern->u.file.escape;
	char *delim = NULL, *enclo = NULL, *esc = NULL;
	size_t d_len = 0, e_len = 0, esc_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sss", &delim, &d_len, &enclo, &e_len, &esc, &esc_len) == SUCCESS) {

		if(!intern->u.file.stream) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
			return;
		}

		switch(ZEND_NUM_ARGS())
		{
		case 3:
			if (esc_len != 1) {
				php_error_docref(NULL, E_WARNING, "escape must be a character");
				RETURN_FALSE;
			}
			escape = esc[0];
			/* no break */
		case 2:
			if (e_len != 1) {
				php_error_docref(NULL, E_WARNING, "enclosure must be a character");
				RETURN_FALSE;
			}
			enclosure = enclo[0];
			/* no break */
		case 1:
			if (d_len != 1) {
				php_error_docref(NULL, E_WARNING, "delimiter must be a character");
				RETURN_FALSE;
			}
			delimiter = delim[0];
			/* no break */
		case 0:
			break;
		}
		spl_filesystem_file_read_csv(intern, delimiter, enclosure, escape, return_value);
	}
}