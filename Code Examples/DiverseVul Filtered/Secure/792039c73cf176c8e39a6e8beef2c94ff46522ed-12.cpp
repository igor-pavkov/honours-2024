static struct sk_buff *l2cap_sock_alloc_skb_cb(struct l2cap_chan *chan,
					       unsigned long len, int nb)
{
	struct sk_buff *skb;
	int err;

	l2cap_chan_unlock(chan);
	skb = bt_skb_send_alloc(chan->sk, len, nb, &err);
	l2cap_chan_lock(chan);

	if (!skb)
		return ERR_PTR(err);

	return skb;
}