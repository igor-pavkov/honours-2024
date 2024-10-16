static void sco_sock_close(struct sock *sk)
{
	sco_sock_clear_timer(sk);
	lock_sock(sk);
	__sco_sock_close(sk);
	release_sock(sk);
	sco_sock_kill(sk);
}