static void irda_disconnect_indication(void *instance, void *sap,
				       LM_REASON reason, struct sk_buff *skb)
{
	struct irda_sock *self;
	struct sock *sk;

	self = instance;

	IRDA_DEBUG(2, "%s(%p)\n", __func__, self);

	/* Don't care about it, but let's not leak it */
	if(skb)
		dev_kfree_skb(skb);

	sk = instance;
	if (sk == NULL) {
		IRDA_DEBUG(0, "%s(%p) : BUG : sk is NULL\n",
			   __func__, self);
		return;
	}

	/* Prevent race conditions with irda_release() and irda_shutdown() */
	bh_lock_sock(sk);
	if (!sock_flag(sk, SOCK_DEAD) && sk->sk_state != TCP_CLOSE) {
		sk->sk_state     = TCP_CLOSE;
		sk->sk_shutdown |= SEND_SHUTDOWN;

		sk->sk_state_change(sk);

		/* Close our TSAP.
		 * If we leave it open, IrLMP put it back into the list of
		 * unconnected LSAPs. The problem is that any incoming request
		 * can then be matched to this socket (and it will be, because
		 * it is at the head of the list). This would prevent any
		 * listening socket waiting on the same TSAP to get those
		 * requests. Some apps forget to close sockets, or hang to it
		 * a bit too long, so we may stay in this dead state long
		 * enough to be noticed...
		 * Note : all socket function do check sk->sk_state, so we are
		 * safe...
		 * Jean II
		 */
		if (self->tsap) {
			irttp_close_tsap(self->tsap);
			self->tsap = NULL;
		}
	}
	bh_unlock_sock(sk);

	/* Note : once we are there, there is not much you want to do
	 * with the socket anymore, apart from closing it.
	 * For example, bind() and connect() won't reset sk->sk_err,
	 * sk->sk_shutdown and sk->sk_flags to valid values...
	 * Jean II
	 */
}