static int raw_setsockopt(struct sock *sk, int level, int optname,
			  char __user *optval, unsigned int optlen)
{
	return -EOPNOTSUPP;
}