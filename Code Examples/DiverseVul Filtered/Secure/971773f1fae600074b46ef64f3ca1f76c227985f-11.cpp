static handler_t mod_wstunnel_check_extension(request_st * const r, void *p_d) {
    plugin_data *p = p_d;
    handler_t rc;

    if (NULL != r->handler_module)
        return HANDLER_GO_ON;
  if (r->http_version > HTTP_VERSION_1_1) {
    if (!r->h2_connect_ext)
        return HANDLER_GO_ON;
  }
  else {
    if (r->http_method != HTTP_METHOD_GET)
        return HANDLER_GO_ON;
    if (r->http_version != HTTP_VERSION_1_1)
        return HANDLER_GO_ON;

    /*
     * Connection: upgrade, keep-alive, ...
     * Upgrade: WebSocket, ...
     */
    const buffer *vb;
    vb = http_header_request_get(r, HTTP_HEADER_UPGRADE, CONST_STR_LEN("Upgrade"));
    if (NULL == vb
        || !http_header_str_contains_token(BUF_PTR_LEN(vb), CONST_STR_LEN("websocket")))
        return HANDLER_GO_ON;
    vb = http_header_request_get(r, HTTP_HEADER_CONNECTION, CONST_STR_LEN("Connection"));
    if (NULL == vb
        || !http_header_str_contains_token(BUF_PTR_LEN(vb), CONST_STR_LEN("upgrade")))
        return HANDLER_GO_ON;
  }

    mod_wstunnel_patch_config(r, p);
    if (NULL == p->conf.gw.exts) return HANDLER_GO_ON;

    rc = gw_check_extension(r, (gw_plugin_data *)p, 1, sizeof(handler_ctx));
    return (HANDLER_GO_ON == rc && r->handler_module == p->self)
      ? wstunnel_handler_setup(r, p)
      : rc;
}