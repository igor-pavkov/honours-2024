int sock_no_connect(struct socket *sock, struct sockaddr *saddr,
		    int len, int flags)
{
	return -EOPNOTSUPP;
}