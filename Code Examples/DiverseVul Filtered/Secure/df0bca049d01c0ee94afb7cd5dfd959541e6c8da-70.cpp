void sk_release_kernel(struct sock *sk)
{
	if (sk == NULL || sk->sk_socket == NULL)
		return;

	sock_hold(sk);
	sock_release(sk->sk_socket);
	release_net(sock_net(sk));
	sock_net_set(sk, get_net(&init_net));
	sock_put(sk);
}