int tcp_ctl(struct socket *so)
{
    Slirp *slirp = so->slirp;
    struct sbuf *sb = &so->so_snd;
    struct gfwd_list *ex_ptr;

    DEBUG_CALL("tcp_ctl");
    DEBUG_ARG("so = %p", so);

    /* TODO: IPv6 */
    if (so->so_faddr.s_addr != slirp->vhost_addr.s_addr) {
        /* Check if it's pty_exec */
        for (ex_ptr = slirp->guestfwd_list; ex_ptr; ex_ptr = ex_ptr->ex_next) {
            if (ex_ptr->ex_fport == so->so_fport &&
                so->so_faddr.s_addr == ex_ptr->ex_addr.s_addr) {
                if (ex_ptr->write_cb) {
                    so->s = -1;
                    so->guestfwd = ex_ptr;
                    return 1;
                }
                DEBUG_MISC(" executing %s", ex_ptr->ex_exec);
                if (ex_ptr->ex_unix)
                    return open_unix(so, ex_ptr->ex_unix);
                else
                    return fork_exec(so, ex_ptr->ex_exec);
            }
        }
    }
    sb->sb_cc = slirp_fmt(sb->sb_wptr, sb->sb_datalen - (sb->sb_wptr - sb->sb_data),
                          "Error: No application configured.\r\n");
    sb->sb_wptr += sb->sb_cc;
    return 0;
}