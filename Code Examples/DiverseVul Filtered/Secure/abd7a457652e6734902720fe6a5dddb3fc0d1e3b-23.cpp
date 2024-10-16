    OVS_REQUIRES(ct->buckets[bucket].lock)
{
    if (is_ftp_ctl(ct_alg_ctl)) {
        /* Keep sequence tracking in sync with the source of the
         * sequence skew. */
        if (ctx->reply != conn->seq_skew_dir) {
            handle_ftp_ctl(ct, ctx, pkt, conn, now, CT_FTP_CTL_OTHER,
                           !!nat_action_info);
            *create_new_conn = conn_update_state(ct, pkt, ctx, &conn, now,
                                                bucket);
        } else {
            *create_new_conn = conn_update_state(ct, pkt, ctx, &conn, now,
                                                bucket);

            if (*create_new_conn == false) {
                handle_ftp_ctl(ct, ctx, pkt, conn, now, CT_FTP_CTL_OTHER,
                               !!nat_action_info);
            }
        }
        return true;
    }
    return false;
}