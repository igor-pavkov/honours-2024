int sock_no_sendmsg_locked(struct sock *sk, struct msghdr *m, size_t len)
{
	return -EOPNOTSUPP;
}