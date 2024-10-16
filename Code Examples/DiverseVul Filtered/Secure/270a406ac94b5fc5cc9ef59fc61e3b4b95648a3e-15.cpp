static void php_csr_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	X509_REQ * csr = (X509_REQ*)rsrc->ptr;
	X509_REQ_free(csr);
}