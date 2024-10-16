int DH_compute_key(unsigned char *key, const BIGNUM *pub_key, DH *dh)
{
#ifdef OPENSSL_FIPS
    if (FIPS_mode() && !(dh->meth->flags & DH_FLAG_FIPS_METHOD)
        && !(dh->flags & DH_FLAG_NON_FIPS_ALLOW)) {
        DHerr(DH_F_DH_COMPUTE_KEY, DH_R_NON_FIPS_METHOD);
        return 0;
    }
#endif
    return dh->meth->compute_key(key, pub_key, dh);
}