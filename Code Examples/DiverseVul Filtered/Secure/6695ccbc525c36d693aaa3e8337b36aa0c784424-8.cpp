static void process_bin_get(conn *c) {
    item *it;

    protocol_binary_response_get* rsp = (protocol_binary_response_get*)c->wbuf;
    char* key = binary_get_key(c);
    size_t nkey = c->binary_header.request.keylen;

    if (settings.verbose > 1) {
        int ii;
        fprintf(stderr, "<%d GET ", c->sfd);
        for (ii = 0; ii < nkey; ++ii) {
            fprintf(stderr, "%c", key[ii]);
        }
        fprintf(stderr, "\n");
    }

    it = item_get(key, nkey);
    if (it) {
        /* the length has two unnecessary bytes ("\r\n") */
        uint16_t keylen = 0;
        uint32_t bodylen = sizeof(rsp->message.body) + (it->nbytes - 2);

        item_update(it);
        pthread_mutex_lock(&c->thread->stats.mutex);
        c->thread->stats.get_cmds++;
        c->thread->stats.slab_stats[it->slabs_clsid].get_hits++;
        pthread_mutex_unlock(&c->thread->stats.mutex);

        MEMCACHED_COMMAND_GET(c->sfd, ITEM_key(it), it->nkey,
                              it->nbytes, ITEM_get_cas(it));

        if (c->cmd == PROTOCOL_BINARY_CMD_GETK) {
            bodylen += nkey;
            keylen = nkey;
        }
        add_bin_header(c, 0, sizeof(rsp->message.body), keylen, bodylen);
        rsp->message.header.response.cas = htonll(ITEM_get_cas(it));

        // add the flags
        rsp->message.body.flags = htonl(strtoul(ITEM_suffix(it), NULL, 10));
        add_iov(c, &rsp->message.body, sizeof(rsp->message.body));

        if (c->cmd == PROTOCOL_BINARY_CMD_GETK) {
            add_iov(c, ITEM_key(it), nkey);
        }

        /* Add the data minus the CRLF */
        add_iov(c, ITEM_data(it), it->nbytes - 2);
        conn_set_state(c, conn_mwrite);
        c->write_and_go = conn_new_cmd;
        /* Remember this command so we can garbage collect it later */
        c->item = it;
    } else {
        pthread_mutex_lock(&c->thread->stats.mutex);
        c->thread->stats.get_cmds++;
        c->thread->stats.get_misses++;
        pthread_mutex_unlock(&c->thread->stats.mutex);

        MEMCACHED_COMMAND_GET(c->sfd, key, nkey, -1, 0);

        if (c->noreply) {
            conn_set_state(c, conn_new_cmd);
        } else {
            if (c->cmd == PROTOCOL_BINARY_CMD_GETK) {
                char *ofs = c->wbuf + sizeof(protocol_binary_response_header);
                add_bin_header(c, PROTOCOL_BINARY_RESPONSE_KEY_ENOENT,
                        0, nkey, nkey);
                memcpy(ofs, key, nkey);
                add_iov(c, ofs, nkey);
                conn_set_state(c, conn_mwrite);
                c->write_and_go = conn_new_cmd;
            } else {
                write_bin_error(c, PROTOCOL_BINARY_RESPONSE_KEY_ENOENT, 0);
            }
        }
    }

    if (settings.detail_enabled) {
        stats_prefix_record_get(key, nkey, NULL != it);
    }
}