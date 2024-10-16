PHP_METHOD(Phar, count)
{
	/* mode can be ignored, maximum depth is 1 */
	long mode;
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &mode) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_LONG(zend_hash_num_elements(&phar_obj->arc.archive->manifest));
}