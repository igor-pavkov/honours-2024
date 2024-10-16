EVP_PKEY *X509_get_pubkey(X509 *x)
{
    if (x == NULL)
        return NULL;
    return X509_PUBKEY_get(x->cert_info.key);
}