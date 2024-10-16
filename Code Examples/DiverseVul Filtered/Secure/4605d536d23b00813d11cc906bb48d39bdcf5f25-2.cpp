void destroy_uploaded_files_hash(TSRMLS_D) /* {{{ */
{
	zend_hash_apply(SG(rfc1867_uploaded_files), (apply_func_t) unlink_filename TSRMLS_CC);
	zend_hash_destroy(SG(rfc1867_uploaded_files));
	FREE_HASHTABLE(SG(rfc1867_uploaded_files));
}