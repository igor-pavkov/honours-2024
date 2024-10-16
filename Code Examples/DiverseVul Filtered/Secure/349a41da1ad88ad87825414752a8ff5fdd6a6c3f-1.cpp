int RSA_generate_key_ex(RSA *rsa, int bits, BIGNUM *e_value, BN_GENCB *cb)
{
#ifdef OPENSSL_FIPS
    if (FIPS_mode() && !(rsa->meth->flags & RSA_FLAG_FIPS_METHOD)
        && !(rsa->flags & RSA_FLAG_NON_FIPS_ALLOW)) {
        RSAerr(RSA_F_RSA_GENERATE_KEY_EX, RSA_R_NON_FIPS_RSA_METHOD);
        return 0;
    }
#endif
    if (rsa->meth->rsa_keygen)
        return rsa->meth->rsa_keygen(rsa, bits, e_value, cb);
#ifdef OPENSSL_FIPS
    if (FIPS_mode())
        return FIPS_rsa_x931_generate_key_ex(rsa, bits, e_value, cb);
#endif
    return rsa_builtin_keygen(rsa, bits, e_value, cb);
}