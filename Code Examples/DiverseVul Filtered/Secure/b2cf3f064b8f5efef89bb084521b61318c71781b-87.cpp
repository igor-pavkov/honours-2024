void phar_object_init(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PharException", phar_exception_methods);
	phar_ce_PharException = zend_register_internal_class_ex(&ce, phar_exception_get_default(), NULL  TSRMLS_CC);

#if HAVE_SPL
	INIT_CLASS_ENTRY(ce, "Phar", php_archive_methods);
	phar_ce_archive = zend_register_internal_class_ex(&ce, spl_ce_RecursiveDirectoryIterator, NULL  TSRMLS_CC);

	zend_class_implements(phar_ce_archive TSRMLS_CC, 2, spl_ce_Countable, zend_ce_arrayaccess);

	INIT_CLASS_ENTRY(ce, "PharData", php_archive_methods);
	phar_ce_data = zend_register_internal_class_ex(&ce, spl_ce_RecursiveDirectoryIterator, NULL  TSRMLS_CC);

	zend_class_implements(phar_ce_data TSRMLS_CC, 2, spl_ce_Countable, zend_ce_arrayaccess);

	INIT_CLASS_ENTRY(ce, "PharFileInfo", php_entry_methods);
	phar_ce_entry = zend_register_internal_class_ex(&ce, spl_ce_SplFileInfo, NULL  TSRMLS_CC);
#else
	INIT_CLASS_ENTRY(ce, "Phar", php_archive_methods);
	phar_ce_archive = zend_register_internal_class(&ce TSRMLS_CC);
	phar_ce_archive->ce_flags |= ZEND_ACC_FINAL_CLASS;

	INIT_CLASS_ENTRY(ce, "PharData", php_archive_methods);
	phar_ce_data = zend_register_internal_class(&ce TSRMLS_CC);
	phar_ce_data->ce_flags |= ZEND_ACC_FINAL_CLASS;
#endif

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