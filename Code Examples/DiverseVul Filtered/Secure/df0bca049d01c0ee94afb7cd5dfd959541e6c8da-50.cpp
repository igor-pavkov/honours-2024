int sock_no_sendmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
		    size_t len)
{
	return -EOPNOTSUPP;
}