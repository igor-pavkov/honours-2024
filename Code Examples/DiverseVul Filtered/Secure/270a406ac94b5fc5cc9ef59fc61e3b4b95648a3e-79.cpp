static void php_x509_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	X509 *x509 = (X509 *)rsrc->ptr;
	X509_free(x509);
}