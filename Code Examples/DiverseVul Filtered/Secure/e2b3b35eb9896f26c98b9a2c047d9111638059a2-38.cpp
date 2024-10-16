static int sk_has_rx_data(struct sock *sk)
{
	struct socket *sock = sk->sk_socket;

	if (sock->ops->peek_len)
		return sock->ops->peek_len(sock);

	return skb_queue_empty(&sk->sk_receive_queue);
}