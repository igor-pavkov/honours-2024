static void tipc_sock_destruct(struct sock *sk)
{
	__skb_queue_purge(&sk->sk_receive_queue);
}