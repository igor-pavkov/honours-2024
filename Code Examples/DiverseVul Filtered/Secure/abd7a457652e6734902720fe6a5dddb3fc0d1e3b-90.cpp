    OVS_REQUIRES(ct->buckets[bucket].lock)
{
    bool create_new_conn = false;
    struct conn lconn;

    if (ctx->icmp_related) {
        pkt->md.ct_state |= CS_RELATED;
        if (ctx->reply) {
            pkt->md.ct_state |= CS_REPLY_DIR;
        }
    } else {
        if ((*conn)->alg_related) {
            pkt->md.ct_state |= CS_RELATED;
        }

        enum ct_update_res res = conn_update(*conn, &ct->buckets[bucket],
                                             pkt, ctx->reply, now);

        switch (res) {
        case CT_UPDATE_VALID:
            pkt->md.ct_state |= CS_ESTABLISHED;
            pkt->md.ct_state &= ~CS_NEW;
            if (ctx->reply) {
                pkt->md.ct_state |= CS_REPLY_DIR;
            }
            break;
        case CT_UPDATE_INVALID:
            pkt->md.ct_state = CS_INVALID;
            break;
        case CT_UPDATE_NEW:
            memcpy(&lconn, *conn, sizeof lconn);
            ct_lock_unlock(&ct->buckets[bucket].lock);
            conn_clean_safe(ct, &lconn, &ct->buckets[bucket], ctx->hash);
            ct_lock_lock(&ct->buckets[bucket].lock);
            create_new_conn = true;
            break;
        default:
            OVS_NOT_REACHED();
        }
    }
    return create_new_conn;
}