static void process_bin_get_or_touch(conn *c) {
    item *it;

    protocol_binary_response_get* rsp = (protocol_binary_response_get*)c->wbuf;
    char* key = binary_get_key(c);
    size_t nkey = c->binary_header.request.keylen;
    int should_touch = (c->cmd == PROTOCOL_BINARY_CMD_TOUCH ||
                        c->cmd == PROTOCOL_BINARY_CMD_GAT ||
                        c->cmd == PROTOCOL_BINARY_CMD_GATK);
    int should_return_key = (c->cmd == PROTOCOL_BINARY_CMD_GETK ||
                             c->cmd == PROTOCOL_BINARY_CMD_GATK);
    int should_return_value = (c->cmd != PROTOCOL_BINARY_CMD_TOUCH);
    bool failed = false;

    if (settings.verbose > 1) {
        fprintf(stderr, "<%d %s ", c->sfd, should_touch ? "TOUCH" : "GET");
        if (fwrite(key, 1, nkey, stderr)) {}
        fputc('\n', stderr);
    }

    if (should_touch) {
        protocol_binary_request_touch *t = binary_get_request(c);
        time_t exptime = ntohl(t->message.body.expiration);

        it = item_touch(key, nkey, realtime(exptime), c);
    } else {
        it = item_get(key, nkey, c, DO_UPDATE);
    }

    if (it) {
        /* the length has two unnecessary bytes ("\r\n") */
        uint16_t keylen = 0;
        uint32_t bodylen = sizeof(rsp->message.body) + (it->nbytes - 2);

        pthread_mutex_lock(&c->thread->stats.mutex);
        if (should_touch) {
            c->thread->stats.touch_cmds++;
            c->thread->stats.slab_stats[ITEM_clsid(it)].touch_hits++;
        } else {
            c->thread->stats.get_cmds++;
            c->thread->stats.lru_hits[it->slabs_clsid]++;
        }
        pthread_mutex_unlock(&c->thread->stats.mutex);

        if (should_touch) {
            MEMCACHED_COMMAND_TOUCH(c->sfd, ITEM_key(it), it->nkey,
                                    it->nbytes, ITEM_get_cas(it));
        } else {
            MEMCACHED_COMMAND_GET(c->sfd, ITEM_key(it), it->nkey,
                                  it->nbytes, ITEM_get_cas(it));
        }

        if (c->cmd == PROTOCOL_BINARY_CMD_TOUCH) {
            bodylen -= it->nbytes - 2;
        } else if (should_return_key) {
            bodylen += nkey;
            keylen = nkey;
        }

        add_bin_header(c, 0, sizeof(rsp->message.body), keylen, bodylen);
        rsp->message.header.response.cas = htonll(ITEM_get_cas(it));

        // add the flags
        FLAGS_CONV(it, rsp->message.body.flags);
        rsp->message.body.flags = htonl(rsp->message.body.flags);
        add_iov(c, &rsp->message.body, sizeof(rsp->message.body));

        if (should_return_key) {
            add_iov(c, ITEM_key(it), nkey);
        }

        if (should_return_value) {
            /* Add the data minus the CRLF */
#ifdef EXTSTORE
            if (it->it_flags & ITEM_HDR) {
                int iovcnt = 4;
                int iovst = c->iovused - 3;
                if (!should_return_key) {
                    iovcnt = 3;
                    iovst = c->iovused - 2;
                }

                if (_get_extstore(c, it, iovst, iovcnt) != 0) {
                    pthread_mutex_lock(&c->thread->stats.mutex);
                    c->thread->stats.get_oom_extstore++;
                    pthread_mutex_unlock(&c->thread->stats.mutex);

                    failed = true;
                }
            } else if ((it->it_flags & ITEM_CHUNKED) == 0) {
                add_iov(c, ITEM_data(it), it->nbytes - 2);
            } else {
                add_chunked_item_iovs(c, it, it->nbytes - 2);
            }
#else
            if ((it->it_flags & ITEM_CHUNKED) == 0) {
                add_iov(c, ITEM_data(it), it->nbytes - 2);
            } else {
                add_chunked_item_iovs(c, it, it->nbytes - 2);
            }
#endif
        }

        if (!failed) {
            conn_set_state(c, conn_mwrite);
            c->write_and_go = conn_new_cmd;
            /* Remember this command so we can garbage collect it later */
#ifdef EXTSTORE
            if ((it->it_flags & ITEM_HDR) != 0 && should_return_value) {
                // Only have extstore clean if header and returning value.
                c->item = NULL;
            } else {
                c->item = it;
            }
#else
            c->item = it;
#endif
        } else {
            item_remove(it);
        }
    } else {
        failed = true;
    }

    if (failed) {
        pthread_mutex_lock(&c->thread->stats.mutex);
        if (should_touch) {
            c->thread->stats.touch_cmds++;
            c->thread->stats.touch_misses++;
        } else {
            c->thread->stats.get_cmds++;
            c->thread->stats.get_misses++;
        }
        pthread_mutex_unlock(&c->thread->stats.mutex);

        if (should_touch) {
            MEMCACHED_COMMAND_TOUCH(c->sfd, key, nkey, -1, 0);
        } else {
            MEMCACHED_COMMAND_GET(c->sfd, key, nkey, -1, 0);
        }

        if (c->noreply) {
            conn_set_state(c, conn_new_cmd);
        } else {
            if (should_return_key) {
                write_bin_miss_response(c, key, nkey);
            } else {
                write_bin_miss_response(c, NULL, 0);
            }
        }
    }

    if (settings.detail_enabled) {
        stats_prefix_record_get(key, nkey, NULL != it);
    }
}