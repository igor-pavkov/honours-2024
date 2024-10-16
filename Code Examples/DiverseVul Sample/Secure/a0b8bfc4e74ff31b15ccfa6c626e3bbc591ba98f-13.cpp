static int pptp_write(struct triton_md_handler_t *h)
{
	struct pptp_conn_t *conn = container_of(h, typeof(*conn), hnd);
	int n;

	while (1) {
		n = write(h->fd, conn->out_buf+conn->out_pos, conn->out_size-conn->out_pos);

		if (n < 0) {
			if (errno == EINTR)
				continue;
			if (errno == EAGAIN)
				n = 0;
			else {
				if (errno != EPIPE) {
					if (conf_verbose)
						log_ppp_info2("pptp: post_msg: %s\n", strerror(errno));
				}
				disconnect(conn);
				return 1;
			}
		}

		conn->out_pos += n;
		if (conn->out_pos == conn->out_size) {
			conn->out_pos = 0;
			conn->out_size = 0;
			triton_md_disable_handler(h, MD_MODE_WRITE);
			return 0;
		}
	}
}