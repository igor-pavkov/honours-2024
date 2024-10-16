static int pvc_setsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	int error;

	lock_sock(sk);
	error = vcc_setsockopt(sock, level, optname, optval, optlen);
	release_sock(sk);
	return error;
}