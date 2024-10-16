static void php_pkey_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	EVP_PKEY *pkey = (EVP_PKEY *)rsrc->ptr;

	assert(pkey != NULL);

	EVP_PKEY_free(pkey);
}