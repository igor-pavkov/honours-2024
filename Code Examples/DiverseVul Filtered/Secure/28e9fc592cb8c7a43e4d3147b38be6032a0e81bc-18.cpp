static int llc_ui_accept(struct socket *sock, struct socket *newsock, int flags)
{
	struct sock *sk = sock->sk, *newsk;
	struct llc_sock *llc, *newllc;
	struct sk_buff *skb;
	int rc = -EOPNOTSUPP;

	dprintk("%s: accepting on %02X\n", __func__,
		llc_sk(sk)->laddr.lsap);
	lock_sock(sk);
	if (unlikely(sk->sk_type != SOCK_STREAM))
		goto out;
	rc = -EINVAL;
	if (unlikely(sock->state != SS_UNCONNECTED ||
		     sk->sk_state != TCP_LISTEN))
		goto out;
	/* wait for a connection to arrive. */
	if (skb_queue_empty(&sk->sk_receive_queue)) {
		rc = llc_wait_data(sk, sk->sk_rcvtimeo);
		if (rc)
			goto out;
	}
	dprintk("%s: got a new connection on %02X\n", __func__,
		llc_sk(sk)->laddr.lsap);
	skb = skb_dequeue(&sk->sk_receive_queue);
	rc = -EINVAL;
	if (!skb->sk)
		goto frees;
	rc = 0;
	newsk = skb->sk;
	/* attach connection to a new socket. */
	llc_ui_sk_init(newsock, newsk);
	sock_reset_flag(newsk, SOCK_ZAPPED);
	newsk->sk_state		= TCP_ESTABLISHED;
	newsock->state		= SS_CONNECTED;
	llc			= llc_sk(sk);
	newllc			= llc_sk(newsk);
	memcpy(&newllc->addr, &llc->addr, sizeof(newllc->addr));
	newllc->link = llc_ui_next_link_no(newllc->laddr.lsap);

	/* put original socket back into a clean listen state. */
	sk->sk_state = TCP_LISTEN;
	sk->sk_ack_backlog--;
	dprintk("%s: ok success on %02X, client on %02X\n", __func__,
		llc_sk(sk)->addr.sllc_sap, newllc->daddr.lsap);
frees:
	kfree_skb(skb);
out:
	release_sock(sk);
	return rc;
}