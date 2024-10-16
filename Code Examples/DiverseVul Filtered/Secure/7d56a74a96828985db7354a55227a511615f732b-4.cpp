const OSSL_PARAM *rc4_hmac_md5_gettable_ctx_params(ossl_unused void *cctx,
                                                   ossl_unused void *provctx)
{
    return rc4_hmac_md5_known_gettable_ctx_params;
}