static int connection_handle_write_state(request_st * const r, connection * const con) {
    do {
        /* only try to write if we have something in the queue */
        if (!chunkqueue_is_empty(&r->write_queue)) {
            if (r->http_version <= HTTP_VERSION_1_1) {
                int rc = connection_handle_write(r, con);
                if (rc != CON_STATE_WRITE) return rc;
            }
        } else if (r->resp_body_finished) {
            connection_set_state(r, CON_STATE_RESPONSE_END);
            return CON_STATE_RESPONSE_END;
        }

        if (r->handler_module && !r->resp_body_finished) {
            const plugin * const p = r->handler_module;
            int rc = p->handle_subrequest(r, p->data);
            switch(rc) {
            case HANDLER_WAIT_FOR_EVENT:
            case HANDLER_FINISHED:
            case HANDLER_GO_ON:
                break;
            case HANDLER_WAIT_FOR_FD:
                /* (In addition to waiting for dispatch from fdwaitqueue,
                 *  HTTP/2 connections may retry more frequently after any
                 *  activity occurs on connection or on other streams) */
                connection_fdwaitqueue_append(con);
                break;
            case HANDLER_COMEBACK:
            default:
                log_error(r->conf.errh, __FILE__, __LINE__,
                  "unexpected subrequest handler ret-value: %d %d",
                  con->fd, rc);
                __attribute_fallthrough__
            case HANDLER_ERROR:
                connection_set_state_error(r, CON_STATE_ERROR);
                return CON_STATE_ERROR;
            }
        }
    } while (r->http_version <= HTTP_VERSION_1_1
             && (!chunkqueue_is_empty(&r->write_queue)
                 ? con->is_writable > 0 && 0 == con->traffic_limit_reached
                 : r->resp_body_finished));

    return CON_STATE_WRITE;
}