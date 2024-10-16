static void complete_incr_bin(conn *c) {
    item *it;
    char *key;
    size_t nkey;

    protocol_binary_response_incr* rsp = (protocol_binary_response_incr*)c->wbuf;
    protocol_binary_request_incr* req = binary_get_request(c);

    assert(c != NULL);
    assert(c->wsize >= sizeof(*rsp));

    /* fix byteorder in the request */
    req->message.body.delta = ntohll(req->message.body.delta);
    req->message.body.initial = ntohll(req->message.body.initial);
    req->message.body.expiration = ntohl(req->message.body.expiration);
    key = binary_get_key(c);
    nkey = c->binary_header.request.keylen;

    if (settings.verbose > 1) {
        int i;
        fprintf(stderr, "incr ");

        for (i = 0; i < nkey; i++) {
            fprintf(stderr, "%c", key[i]);
        }
        fprintf(stderr, " %lld, %llu, %d\n",
                (long long)req->message.body.delta,
                (long long)req->message.body.initial,
                req->message.body.expiration);
    }

    it = item_get(key, nkey);
    if (it && (c->binary_header.request.cas == 0 ||
               c->binary_header.request.cas == ITEM_get_cas(it))) {
        /* Weird magic in add_delta forces me to pad here */
        char tmpbuf[INCR_MAX_STORAGE_LEN];
        protocol_binary_response_status st = PROTOCOL_BINARY_RESPONSE_SUCCESS;

        switch(add_delta(c, it, c->cmd == PROTOCOL_BINARY_CMD_INCREMENT,
                         req->message.body.delta, tmpbuf)) {
        case OK:
            break;
        case NON_NUMERIC:
            st = PROTOCOL_BINARY_RESPONSE_DELTA_BADVAL;
            break;
        case EOM:
            st = PROTOCOL_BINARY_RESPONSE_ENOMEM;
            break;
        }

        if (st != PROTOCOL_BINARY_RESPONSE_SUCCESS) {
            write_bin_error(c, st, 0);
        } else {
            rsp->message.body.value = htonll(strtoull(tmpbuf, NULL, 10));
            c->cas = ITEM_get_cas(it);
            write_bin_response(c, &rsp->message.body, 0, 0,
                               sizeof(rsp->message.body.value));
        }

        item_remove(it);         /* release our reference */
    } else if (!it && req->message.body.expiration != 0xffffffff) {
        /* Save some room for the response */
        rsp->message.body.value = htonll(req->message.body.initial);
        it = item_alloc(key, nkey, 0, realtime(req->message.body.expiration),
                        INCR_MAX_STORAGE_LEN);

        if (it != NULL) {
            snprintf(ITEM_data(it), INCR_MAX_STORAGE_LEN, "%llu",
                     (unsigned long long)req->message.body.initial);

            if (store_item(it, NREAD_SET, c)) {
                c->cas = ITEM_get_cas(it);
                write_bin_response(c, &rsp->message.body, 0, 0, sizeof(rsp->message.body.value));
            } else {
                write_bin_error(c, PROTOCOL_BINARY_RESPONSE_NOT_STORED, 0);
            }
            item_remove(it);         /* release our reference */
        } else {
            write_bin_error(c, PROTOCOL_BINARY_RESPONSE_ENOMEM, 0);
        }
    } else if (it) {
        /* incorrect CAS */
        item_remove(it);         /* release our reference */
        write_bin_error(c, PROTOCOL_BINARY_RESPONSE_KEY_EEXISTS, 0);
    } else {

        pthread_mutex_lock(&c->thread->stats.mutex);
        if (c->cmd == PROTOCOL_BINARY_CMD_INCREMENT) {
            c->thread->stats.incr_misses++;
        } else {
            c->thread->stats.decr_misses++;
        }
        pthread_mutex_unlock(&c->thread->stats.mutex);

        write_bin_error(c, PROTOCOL_BINARY_RESPONSE_KEY_ENOENT, 0);
    }
}