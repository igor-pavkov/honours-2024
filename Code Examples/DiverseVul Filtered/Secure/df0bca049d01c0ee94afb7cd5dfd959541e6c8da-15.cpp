void sk_send_sigurg(struct sock *sk)
{
	if (sk->sk_socket && sk->sk_socket->file)
		if (send_sigurg(&sk->sk_socket->file->f_owner))
			sk_wake_async(sk, SOCK_WAKE_URG, POLL_PRI);
}