static void php_openssl_csr_free(zend_resource *rsrc)
{
	X509_REQ * csr = (X509_REQ*)rsrc->ptr;
	X509_REQ_free(csr);
}