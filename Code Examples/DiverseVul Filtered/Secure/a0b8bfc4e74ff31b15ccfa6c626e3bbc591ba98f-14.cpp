static int pptp_read(struct triton_md_handler_t *h)
{
	struct pptp_conn_t *conn=container_of(h,typeof(*conn),hnd);
	struct pptp_header *hdr=(struct pptp_header *)conn->in_buf;
	int n;

	while(1) {
		n = read(h->fd, conn->in_buf + conn->in_size, PPTP_CTRL_SIZE_MAX - conn->in_size);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			if (errno == EAGAIN)
				return 0;
			log_ppp_error("pptp: read: %s\n",strerror(errno));
			goto drop;
		}
		if (n == 0) {
			if (conf_verbose)
				log_ppp_info2("pptp: disconnect by peer\n");
			goto drop;
		}
		conn->in_size += n;
		if (conn->in_size >= sizeof(*hdr)) {
			if (hdr->magic != htonl(PPTP_MAGIC)) {
				log_ppp_error("pptp: invalid magic\n");
				goto drop;
			}
			if (ntohs(hdr->length) >= PPTP_CTRL_SIZE_MAX) {
				log_ppp_error("pptp: message is too long\n");
				goto drop;
			}
			if (ntohs(hdr->length) > conn->in_size)
				continue;
			if (ntohs(hdr->length) <= conn->in_size) {
				if (ntohs(hdr->length) != PPTP_CTRL_SIZE(ntohs(hdr->ctrl_type))) {
					log_ppp_error("pptp: invalid message length\n");
					goto drop;
				}
				if (process_packet(conn))
					goto drop;
				conn->in_size -= ntohs(hdr->length);
				if (conn->in_size)
					memmove(conn->in_buf, conn->in_buf + ntohs(hdr->length), conn->in_size);
			}
		}
	}
drop:
	disconnect(conn);
	return 1;
}