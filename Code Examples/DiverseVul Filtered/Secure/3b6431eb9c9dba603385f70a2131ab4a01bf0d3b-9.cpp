static int hook_note_digest_auth_failure(request_rec *r, const char *auth_type)
{
    request_rec *mainreq;
    digest_header_rec *resp;
    digest_config_rec *conf;

    if (strcasecmp(auth_type, "Digest"))
        return DECLINED;

    /* get the client response and mark */

    mainreq = r;
    while (mainreq->main != NULL) {
        mainreq = mainreq->main;
    }
    while (mainreq->prev != NULL) {
        mainreq = mainreq->prev;
    }
    resp = (digest_header_rec *) ap_get_module_config(mainreq->request_config,
                                                      &auth_digest_module);
    resp->needed_auth = 1;


    /* get our conf */

    conf = (digest_config_rec *) ap_get_module_config(r->per_dir_config,
                                                      &auth_digest_module);

    note_digest_auth_failure(r, conf, resp, 0);

    return OK;
}