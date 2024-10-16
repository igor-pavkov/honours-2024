int mod_wstunnel_frame_send(handler_ctx *hctx, mod_wstunnel_frame_type_t type,
                             const char *payload, size_t siz) {
  #ifdef _MOD_WEBSOCKET_SPEC_RFC_6455_
    if (hctx->hybivers >= 8) return send_rfc_6455(hctx, type, payload, siz);
  #endif /* _MOD_WEBSOCKET_SPEC_RFC_6455_ */
  #ifdef _MOD_WEBSOCKET_SPEC_IETF_00_
    if (0 == hctx->hybivers) return send_ietf_00(hctx, type, payload, siz);
  #endif /* _MOD_WEBSOCKET_SPEC_IETF_00_ */
    return -1;
}