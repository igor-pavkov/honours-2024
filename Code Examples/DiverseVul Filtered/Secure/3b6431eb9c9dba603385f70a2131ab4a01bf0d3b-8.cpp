static void *create_digest_dir_config(apr_pool_t *p, char *dir)
{
    digest_config_rec *conf;

    if (dir == NULL) {
        return NULL;
    }

    conf = (digest_config_rec *) apr_pcalloc(p, sizeof(digest_config_rec));
    if (conf) {
        conf->qop_list       = apr_array_make(p, 2, sizeof(char *));
        conf->nonce_lifetime = DFLT_NONCE_LIFE;
        conf->dir_name       = apr_pstrdup(p, dir);
        conf->algorithm      = DFLT_ALGORITHM;
    }

    return conf;
}