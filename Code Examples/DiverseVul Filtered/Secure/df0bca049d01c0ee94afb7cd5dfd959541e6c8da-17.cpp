int sock_no_setsockopt(struct socket *sock, int level, int optname,
		    char __user *optval, int optlen)
{
	return -EOPNOTSUPP;
}