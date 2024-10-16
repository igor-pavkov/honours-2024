static int ieee802154_dgram_deliver(struct net_device *dev, struct sk_buff *skb)
{
	struct sock *sk, *prev = NULL;
	int ret = NET_RX_SUCCESS;
	__le16 pan_id, short_addr;
	__le64 hw_addr;

	/* Data frame processing */
	BUG_ON(dev->type != ARPHRD_IEEE802154);

	pan_id = dev->ieee802154_ptr->pan_id;
	short_addr = dev->ieee802154_ptr->short_addr;
	hw_addr = dev->ieee802154_ptr->extended_addr;

	read_lock(&dgram_lock);
	sk_for_each(sk, &dgram_head) {
		if (ieee802154_match_sock(hw_addr, pan_id, short_addr,
					  dgram_sk(sk))) {
			if (prev) {
				struct sk_buff *clone;

				clone = skb_clone(skb, GFP_ATOMIC);
				if (clone)
					dgram_rcv_skb(prev, clone);
			}

			prev = sk;
		}
	}

	if (prev) {
		dgram_rcv_skb(prev, skb);
	} else {
		kfree_skb(skb);
		ret = NET_RX_DROP;
	}
	read_unlock(&dgram_lock);

	return ret;
}