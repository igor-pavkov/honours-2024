EVP_PKEY *X509_get0_pubkey(const X509 *x)
{
    if (x == NULL)
        return NULL;
    return X509_PUBKEY_get0(x->cert_info.key);
}