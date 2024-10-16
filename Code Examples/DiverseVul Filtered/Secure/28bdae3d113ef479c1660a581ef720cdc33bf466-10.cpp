static int send_fd(int sock_fd, int fd)
{
	int retval;
	struct msghdr msg;
	struct cmsghdr *p_cmsg;
	struct iovec vec;
	size_t cmsgbuf[CMSG_SPACE(sizeof(fd)) / sizeof(size_t)];
	int *p_fds;
	char sendchar = 0;

	msg.msg_control = cmsgbuf;
	msg.msg_controllen = sizeof(cmsgbuf);
	p_cmsg = CMSG_FIRSTHDR(&msg);
	p_cmsg->cmsg_level = SOL_SOCKET;
	p_cmsg->cmsg_type = SCM_RIGHTS;
	p_cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
	p_fds = (int *) CMSG_DATA(p_cmsg);
	*p_fds = fd;
	msg.msg_controllen = p_cmsg->cmsg_len;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;
	/* "To pass file descriptors or credentials you need to send/read at
	 * least one byte" (man 7 unix) */
	vec.iov_base = &sendchar;
	vec.iov_len = sizeof(sendchar);
	while ((retval = sendmsg(sock_fd, &msg, 0)) == -1 && errno == EINTR);
	if (retval != 1) {
		perror("sending file descriptor");
		return -1;
	}
	return 0;
}