static const char *gen_nonce(apr_pool_t *p, apr_time_t now, const char *opaque,
                             const server_rec *server,
                             const digest_config_rec *conf)
{
    char *nonce = apr_palloc(p, NONCE_LEN+1);
    time_rec t;

    if (conf->nonce_lifetime != 0) {
        t.time = now;
    }
    else if (otn_counter) {
        /* this counter is not synch'd, because it doesn't really matter
         * if it counts exactly.
         */
        t.time = (*otn_counter)++;
    }
    else {
        /* XXX: WHAT IS THIS CONSTANT? */
        t.time = 42;
    }
    apr_base64_encode_binary(nonce, t.arr, sizeof(t.arr));
    gen_nonce_hash(nonce+NONCE_TIME_LEN, nonce, opaque, server, conf);

    return nonce;
}