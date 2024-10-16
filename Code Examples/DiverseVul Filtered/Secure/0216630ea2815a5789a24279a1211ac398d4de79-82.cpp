static void php_openssl_x509_free(zend_resource *rsrc)
{
	X509 *x509 = (X509 *)rsrc->ptr;
	X509_free(x509);
}