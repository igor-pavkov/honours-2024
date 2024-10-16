static void ieee802154_raw_deliver(struct net_device *dev, struct sk_buff *skb)
{
	struct sock *sk;

	read_lock(&raw_lock);
	sk_for_each(sk, &raw_head) {
		bh_lock_sock(sk);
		if (!sk->sk_bound_dev_if ||
		    sk->sk_bound_dev_if == dev->ifindex) {
			struct sk_buff *clone;

			clone = skb_clone(skb, GFP_ATOMIC);
			if (clone)
				raw_rcv_skb(sk, clone);
		}
		bh_unlock_sock(sk);
	}
	read_unlock(&raw_lock);
}