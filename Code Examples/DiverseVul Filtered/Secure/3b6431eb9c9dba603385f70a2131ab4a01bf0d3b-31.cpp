static int parse_hdr_and_update_nc(request_rec *r)
{
    digest_header_rec *resp;
    int res;

    if (!ap_is_initial_req(r)) {
        return DECLINED;
    }

    resp = apr_pcalloc(r->pool, sizeof(digest_header_rec));
    resp->raw_request_uri = r->unparsed_uri;
    resp->psd_request_uri = &r->parsed_uri;
    resp->needed_auth = 0;
    resp->method = r->method;
    ap_set_module_config(r->request_config, &auth_digest_module, resp);

    res = get_digest_rec(r, resp);
    resp->client = get_client(resp->opaque_num, r);
    if (res == OK && resp->client) {
        resp->client->nonce_count++;
    }

    return DECLINED;
}