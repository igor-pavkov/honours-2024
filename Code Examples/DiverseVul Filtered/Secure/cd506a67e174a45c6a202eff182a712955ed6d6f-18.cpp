void processRequest(struct reqelem * req)
{
	ssize_t n, r;
	unsigned char buf[2048];
	const unsigned char * p;

	n = read(req->socket, buf, sizeof(buf));
	if(n<0) {
		if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
			return;	/* try again later */
		syslog(LOG_ERR, "(s=%d) processRequest(): read(): %m", req->socket);
		goto error;
	}
	if(n==0) {
		syslog(LOG_INFO, "(s=%d) request connection closed", req->socket);
		goto error;
	}
	p = buf;
	while (n > 0)
	{
		r = processRequestSub(req, p, n);
		if (r < 0)
			goto error;
		p += r;
		n -= r;
	}
	return;
error:
	close(req->socket);
	req->socket = -1;
}