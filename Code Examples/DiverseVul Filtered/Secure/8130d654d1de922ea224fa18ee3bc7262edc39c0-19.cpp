int X509_cmp(const X509 *a, const X509 *b)
{
    int rv = 0;

    if (a == b) /* for efficiency */
        return 0;

    /* attempt to compute cert hash */
    (void)X509_check_purpose((X509 *)a, -1, 0);
    (void)X509_check_purpose((X509 *)b, -1, 0);

    if ((a->ex_flags & EXFLAG_NO_FINGERPRINT) == 0
            && (b->ex_flags & EXFLAG_NO_FINGERPRINT) == 0)
        rv = memcmp(a->sha1_hash, b->sha1_hash, SHA_DIGEST_LENGTH);
    if (rv != 0)
        return rv < 0 ? -1 : 1;

    /* Check for match against stored encoding too */
    if (!a->cert_info.enc.modified && !b->cert_info.enc.modified) {
        if (a->cert_info.enc.len < b->cert_info.enc.len)
            return -1;
        if (a->cert_info.enc.len > b->cert_info.enc.len)
            return 1;
        rv = memcmp(a->cert_info.enc.enc,
                    b->cert_info.enc.enc, a->cert_info.enc.len);
    }
    return rv < 0 ? -1 : rv > 0;
}