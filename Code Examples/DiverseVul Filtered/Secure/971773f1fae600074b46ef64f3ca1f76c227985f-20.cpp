int mod_wstunnel_frame_recv(handler_ctx *hctx) {
  #ifdef _MOD_WEBSOCKET_SPEC_RFC_6455_
    if (hctx->hybivers >= 8) return recv_rfc_6455(hctx);
  #endif /* _MOD_WEBSOCKET_SPEC_RFC_6455_ */
  #ifdef _MOD_WEBSOCKET_SPEC_IETF_00_
    if (0 == hctx->hybivers) return recv_ietf_00(hctx);
  #endif /* _MOD_WEBSOCKET_SPEC_IETF_00_ */
    return -1;
}