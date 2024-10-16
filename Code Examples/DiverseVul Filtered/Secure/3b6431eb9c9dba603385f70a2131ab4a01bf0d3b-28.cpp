static void register_hooks(apr_pool_t *p)
{
    static const char * const cfgPost[]={ "http_core.c", NULL };
    static const char * const parsePre[]={ "mod_proxy.c", NULL };

    ap_hook_pre_config(pre_init, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_post_config(initialize_module, NULL, cfgPost, APR_HOOK_MIDDLE);
    ap_hook_child_init(initialize_child, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_post_read_request(parse_hdr_and_update_nc, parsePre, NULL, APR_HOOK_MIDDLE);
    ap_hook_check_authn(authenticate_digest_user, NULL, NULL, APR_HOOK_MIDDLE,
                        AP_AUTH_INTERNAL_PER_CONF);

    ap_hook_fixups(add_auth_info, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_note_auth_failure(hook_note_digest_auth_failure, NULL, NULL,
                              APR_HOOK_MIDDLE);

}