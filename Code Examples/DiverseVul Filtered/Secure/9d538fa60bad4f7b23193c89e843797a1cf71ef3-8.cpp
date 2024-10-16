int sock_no_accept(struct socket *sock, struct socket *newsock, int flags,
		   bool kern)
{
	return -EOPNOTSUPP;
}