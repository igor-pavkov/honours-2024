static void raw_rcv(struct sk_buff *skb, void *data)
{
	struct sock *sk = (struct sock *)data;
	struct raw_sock *ro = raw_sk(sk);
	struct sockaddr_can *addr;

	/* check the received tx sock reference */
	if (!ro->recv_own_msgs && skb->sk == sk)
		return;

	/* clone the given skb to be able to enqueue it into the rcv queue */
	skb = skb_clone(skb, GFP_ATOMIC);
	if (!skb)
		return;

	/*
	 *  Put the datagram to the queue so that raw_recvmsg() can
	 *  get it from there.  We need to pass the interface index to
	 *  raw_recvmsg().  We pass a whole struct sockaddr_can in skb->cb
	 *  containing the interface index.
	 */

	BUILD_BUG_ON(sizeof(skb->cb) < sizeof(struct sockaddr_can));
	addr = (struct sockaddr_can *)skb->cb;
	memset(addr, 0, sizeof(*addr));
	addr->can_family  = AF_CAN;
	addr->can_ifindex = skb->dev->ifindex;

	if (sock_queue_rcv_skb(sk, skb) < 0)
		kfree_skb(skb);
}