TRIGGER_FUNC(mod_wstunnel_handle_trigger) {
    const plugin_data * const p = p_d;
    const unix_time64_t cur_ts = log_monotonic_secs + 1;

    gw_handle_trigger(srv, p_d);

    for (connection *con = srv->conns; con; con = con->next) {
        request_st * const r = &con->request;
        handler_ctx *hctx = r->plugin_ctx[p->id];
        if (NULL == hctx || r->handler_module != p->self)
            continue;

        if (hctx->gw.state != GW_STATE_WRITE && hctx->gw.state != GW_STATE_READ)
            continue;

        if (cur_ts - con->read_idle_ts > r->conf.max_read_idle) {
            DEBUG_LOG_INFO("timeout client (fd=%d)", con->fd);
            mod_wstunnel_frame_send(hctx,MOD_WEBSOCKET_FRAME_TYPE_CLOSE,NULL,0);
            gw_handle_request_reset(r, p_d);
            joblist_append(con);
            /* avoid server.c closing connection with error due to max_read_idle
             * (might instead run joblist after plugins_call_handle_trigger())*/
            con->read_idle_ts = cur_ts;
            continue;
        }

        if (0 != hctx->hybivers
            && hctx->conf.ping_interval > 0
            && (int32_t)hctx->conf.ping_interval + hctx->ping_ts < cur_ts) {
            hctx->ping_ts = cur_ts;
            mod_wstunnel_frame_send(hctx, MOD_WEBSOCKET_FRAME_TYPE_PING, CONST_STR_LEN("ping"));
            joblist_append(con);
            continue;
        }
    }

    return HANDLER_GO_ON;
}