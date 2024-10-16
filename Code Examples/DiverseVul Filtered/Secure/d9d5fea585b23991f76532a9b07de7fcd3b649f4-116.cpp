static void *wsgi_create_server_config(apr_pool_t *p, server_rec *s)
{
    WSGIServerConfig *config = NULL;

    config = newWSGIServerConfig(p);

    return config;
}