int sock_no_recvmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
		    size_t len, int flags)
{
	return -EOPNOTSUPP;
}