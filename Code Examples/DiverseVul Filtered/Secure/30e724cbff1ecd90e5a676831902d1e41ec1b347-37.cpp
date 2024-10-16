static bool vfswrap_init_asys_ctx(struct smbXsrv_connection *conn)
{
	int ret;
	int fd;

	if (conn->asys_ctx != NULL) {
		return true;
	}
	ret = asys_context_init(&conn->asys_ctx, aio_pending_size);
	if (ret != 0) {
		DEBUG(1, ("asys_context_init failed: %s\n", strerror(ret)));
		return false;
	}

	fd = asys_signalfd(conn->asys_ctx);

	set_blocking(fd, false);

	conn->asys_fde = tevent_add_fd(conn->ev_ctx, conn, fd,
				       TEVENT_FD_READ,
				       vfswrap_asys_finished,
				       conn->asys_ctx);
	if (conn->asys_fde == NULL) {
		DEBUG(1, ("tevent_add_fd failed\n"));
		asys_context_destroy(conn->asys_ctx);
		conn->asys_ctx = NULL;
		return false;
	}
	return true;
}