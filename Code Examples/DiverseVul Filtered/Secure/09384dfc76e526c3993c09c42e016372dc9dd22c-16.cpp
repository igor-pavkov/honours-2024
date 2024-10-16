static int irda_data_indication(void *instance, void *sap, struct sk_buff *skb)
{
	struct irda_sock *self;
	struct sock *sk;
	int err;

	IRDA_DEBUG(3, "%s()\n", __func__);

	self = instance;
	sk = instance;

	err = sock_queue_rcv_skb(sk, skb);
	if (err) {
		IRDA_DEBUG(1, "%s(), error: no more mem!\n", __func__);
		self->rx_flow = FLOW_STOP;

		/* When we return error, TTP will need to requeue the skb */
		return err;
	}

	return 0;
}