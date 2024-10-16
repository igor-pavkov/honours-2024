void X509V3_conf_free(CONF_VALUE *conf)
{
    if (!conf)
        return;
    OPENSSL_free(conf->name);
    OPENSSL_free(conf->value);
    OPENSSL_free(conf->section);
    OPENSSL_free(conf);
}