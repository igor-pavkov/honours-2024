static void sock_disable_timestamp(struct sock *sk)
{
	if (sock_flag(sk, SOCK_TIMESTAMP)) {
		sock_reset_flag(sk, SOCK_TIMESTAMP);
		net_disable_timestamp();
	}
}