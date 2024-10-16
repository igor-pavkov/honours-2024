struct sk_buff *pep_read(struct sock *sk)
{
	struct sk_buff *skb = skb_dequeue(&sk->sk_receive_queue);

	if (sk->sk_state == TCP_ESTABLISHED)
		pipe_grant_credits(sk, GFP_ATOMIC);
	return skb;
}