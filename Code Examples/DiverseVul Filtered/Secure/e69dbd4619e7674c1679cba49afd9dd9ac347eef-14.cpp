static int dgram_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	switch (cmd) {
	case SIOCOUTQ:
	{
		int amount = sk_wmem_alloc_get(sk);

		return put_user(amount, (int __user *)arg);
	}

	case SIOCINQ:
	{
		struct sk_buff *skb;
		unsigned long amount;

		amount = 0;
		spin_lock_bh(&sk->sk_receive_queue.lock);
		skb = skb_peek(&sk->sk_receive_queue);
		if (skb) {
			/* We will only return the amount
			 * of this packet since that is all
			 * that will be read.
			 */
			amount = skb->len - ieee802154_hdr_length(skb);
		}
		spin_unlock_bh(&sk->sk_receive_queue.lock);
		return put_user(amount, (int __user *)arg);
	}
	}

	return -ENOIOCTLCMD;
}