int mod_wstunnel_plugin_init(plugin *p) {
    p->version           = LIGHTTPD_VERSION_ID;
    p->name              = "wstunnel";
    p->init              = mod_wstunnel_init;
    p->cleanup           = gw_free;
    p->set_defaults      = mod_wstunnel_set_defaults;
    p->handle_request_reset = gw_handle_request_reset;
    p->handle_uri_clean  = mod_wstunnel_check_extension;
    p->handle_subrequest = gw_handle_subrequest;
    p->handle_trigger    = mod_wstunnel_handle_trigger;
    p->handle_waitpid    = gw_handle_waitpid_cb;
    return 0;
}