static int X509_get_signature_nid(const X509 *x)
{
	return OBJ_obj2nid(x->sig_alg->algorithm);
}