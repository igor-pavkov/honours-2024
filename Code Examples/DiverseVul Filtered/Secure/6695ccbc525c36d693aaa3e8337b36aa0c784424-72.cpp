static void write_bin_error(conn *c, protocol_binary_response_status err, int swallow) {
    const char *errstr = "Unknown error";
    size_t len;

    switch (err) {
    case PROTOCOL_BINARY_RESPONSE_ENOMEM:
        errstr = "Out of memory";
        break;
    case PROTOCOL_BINARY_RESPONSE_UNKNOWN_COMMAND:
        errstr = "Unknown command";
        break;
    case PROTOCOL_BINARY_RESPONSE_KEY_ENOENT:
        errstr = "Not found";
        break;
    case PROTOCOL_BINARY_RESPONSE_EINVAL:
        errstr = "Invalid arguments";
        break;
    case PROTOCOL_BINARY_RESPONSE_KEY_EEXISTS:
        errstr = "Data exists for key.";
        break;
    case PROTOCOL_BINARY_RESPONSE_E2BIG:
        errstr = "Too large.";
        break;
    case PROTOCOL_BINARY_RESPONSE_DELTA_BADVAL:
        errstr = "Non-numeric server-side value for incr or decr";
        break;
    case PROTOCOL_BINARY_RESPONSE_NOT_STORED:
        errstr = "Not stored.";
        break;
    case PROTOCOL_BINARY_RESPONSE_AUTH_ERROR:
        errstr = "Auth failure.";
        break;
    default:
        assert(false);
        errstr = "UNHANDLED ERROR";
        fprintf(stderr, ">%d UNHANDLED ERROR: %d\n", c->sfd, err);
    }

    if (settings.verbose > 1) {
        fprintf(stderr, ">%d Writing an error: %s\n", c->sfd, errstr);
    }

    len = strlen(errstr);
    add_bin_header(c, err, 0, 0, len);
    if (len > 0) {
        add_iov(c, errstr, len);
    }
    conn_set_state(c, conn_mwrite);
    if(swallow > 0) {
        c->sbytes = swallow;
        c->write_and_go = conn_swallow;
    } else {
        c->write_and_go = conn_new_cmd;
    }
}