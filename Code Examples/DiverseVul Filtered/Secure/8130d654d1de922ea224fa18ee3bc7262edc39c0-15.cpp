unsigned long X509_issuer_name_hash(X509 *x)
{
    return X509_NAME_hash_ex(x->cert_info.issuer, NULL, NULL, NULL);
}