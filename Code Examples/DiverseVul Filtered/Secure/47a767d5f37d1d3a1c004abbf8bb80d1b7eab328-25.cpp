char *am_get_endpoint_url(request_rec *r)
{
    am_dir_cfg_rec *cfg = am_get_dir_cfg(r);

    return ap_construct_url(r->pool, cfg->endpoint_path, r);
}