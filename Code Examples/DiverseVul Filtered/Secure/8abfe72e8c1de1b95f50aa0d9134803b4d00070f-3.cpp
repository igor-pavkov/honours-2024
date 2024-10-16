static int dsa_sign_setup_no_digest(DSA *dsa, BN_CTX *ctx_in,
                                    BIGNUM **kinvp, BIGNUM **rp)
{
    return dsa_sign_setup(dsa, ctx_in, kinvp, rp, NULL, 0);
}