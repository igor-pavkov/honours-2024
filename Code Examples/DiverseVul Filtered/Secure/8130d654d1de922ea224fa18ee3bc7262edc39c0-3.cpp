int X509_CRL_match(const X509_CRL *a, const X509_CRL *b)
{
    int rv;

    if ((a->flags & EXFLAG_NO_FINGERPRINT) == 0
            && (b->flags & EXFLAG_NO_FINGERPRINT) == 0)
        rv = memcmp(a->sha1_hash, b->sha1_hash, SHA_DIGEST_LENGTH);
    else
        return -2;

    return rv < 0 ? -1 : rv > 0;
}