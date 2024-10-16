void phar_object_init(void) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PharException", phar_exception_methods);
	phar_ce_PharException = zend_register_internal_class_ex(&ce, zend_ce_exception);

	INIT_CLASS_ENTRY(ce, "Phar", php_archive_methods);
	phar_ce_archive = zend_register_internal_class_ex(&ce, spl_ce_RecursiveDirectoryIterator);

	zend_class_implements(phar_ce_archive, 2, zend_ce_countable, zend_ce_arrayaccess);

	INIT_CLASS_ENTRY(ce, "PharData", php_data_methods);
	phar_ce_data = zend_register_internal_class_ex(&ce, spl_ce_RecursiveDirectoryIterator);

	zend_class_implements(phar_ce_data, 2, zend_ce_countable, zend_ce_arrayaccess);

	INIT_CLASS_ENTRY(ce, "PharFileInfo", php_entry_methods);
	phar_ce_entry = zend_register_internal_class_ex(&ce, spl_ce_SplFileInfo);

	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "BZ2", PHAR_ENT_COMPRESSED_BZ2)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "GZ", PHAR_ENT_COMPRESSED_GZ)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "NONE", PHAR_ENT_COMPRESSED_NONE)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "PHAR", PHAR_FORMAT_PHAR)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "TAR", PHAR_FORMAT_TAR)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "ZIP", PHAR_FORMAT_ZIP)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "COMPRESSED", PHAR_ENT_COMPRESSION_MASK)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "PHP", PHAR_MIME_PHP)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "PHPS", PHAR_MIME_PHPS)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "MD5", PHAR_SIG_MD5)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "OPENSSL", PHAR_SIG_OPENSSL)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "SHA1", PHAR_SIG_SHA1)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "SHA256", PHAR_SIG_SHA256)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "SHA512", PHAR_SIG_SHA512)
}