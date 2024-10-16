static int dgram_recvmsg(struct sock *sk, struct msghdr *msg, size_t len,
			 int noblock, int flags, int *addr_len)
{
	size_t copied = 0;
	int err = -EOPNOTSUPP;
	struct sk_buff *skb;
	struct dgram_sock *ro = dgram_sk(sk);
	DECLARE_SOCKADDR(struct sockaddr_ieee802154 *, saddr, msg->msg_name);

	skb = skb_recv_datagram(sk, flags, noblock, &err);
	if (!skb)
		goto out;

	copied = skb->len;
	if (len < copied) {
		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}

	/* FIXME: skip headers if necessary ?! */
	err = skb_copy_datagram_msg(skb, 0, msg, copied);
	if (err)
		goto done;

	sock_recv_ts_and_drops(msg, sk, skb);

	if (saddr) {
		/* Clear the implicit padding in struct sockaddr_ieee802154
		 * (16 bits between 'family' and 'addr') and in struct
		 * ieee802154_addr_sa (16 bits at the end of the structure).
		 */
		memset(saddr, 0, sizeof(*saddr));

		saddr->family = AF_IEEE802154;
		ieee802154_addr_to_sa(&saddr->addr, &mac_cb(skb)->source);
		*addr_len = sizeof(*saddr);
	}

	if (ro->want_lqi) {
		err = put_cmsg(msg, SOL_IEEE802154, WPAN_WANTLQI,
			       sizeof(uint8_t), &(mac_cb(skb)->lqi));
		if (err)
			goto done;
	}

	if (flags & MSG_TRUNC)
		copied = skb->len;
done:
	skb_free_datagram(sk, skb);
out:
	if (err)
		return err;
	return copied;
}