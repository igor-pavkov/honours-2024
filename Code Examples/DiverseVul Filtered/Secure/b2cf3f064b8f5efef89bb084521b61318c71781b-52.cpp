PHP_METHOD(PharFileInfo, isCompressed)
{
	/* a number that is not Phar::GZ or Phar::BZ2 */
	long method = 9021976;
	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &method) == FAILURE) {
		return;
	}

	switch (method) {
		case 9021976:
			RETURN_BOOL(entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSION_MASK);
		case PHAR_ENT_COMPRESSED_GZ:
			RETURN_BOOL(entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ);
		case PHAR_ENT_COMPRESSED_BZ2:
			RETURN_BOOL(entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2);
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
				"Unknown compression type specified"); \
	}
}