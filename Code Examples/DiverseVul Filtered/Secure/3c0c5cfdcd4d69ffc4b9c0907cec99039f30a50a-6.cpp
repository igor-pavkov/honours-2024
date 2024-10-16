static int pvc_getsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	int error;

	lock_sock(sk);
	error = vcc_getsockopt(sock, level, optname, optval, optlen);
	release_sock(sk);
	return error;
}