void sock_enable_timestamp(struct sock *sk)
{
	if (!sock_flag(sk, SOCK_TIMESTAMP)) {
		sock_set_flag(sk, SOCK_TIMESTAMP);
		net_enable_timestamp();
	}
}