static void aun_data_available(struct sock *sk, int slen)
{
	int err;
	struct sk_buff *skb;
	unsigned char *data;
	struct aunhdr *ah;
	struct iphdr *ip;
	size_t len;

	while ((skb = skb_recv_datagram(sk, 0, 1, &err)) == NULL) {
		if (err == -EAGAIN) {
			printk(KERN_ERR "AUN: no data available?!");
			return;
		}
		printk(KERN_DEBUG "AUN: recvfrom() error %d\n", -err);
	}

	data = skb_transport_header(skb) + sizeof(struct udphdr);
	ah = (struct aunhdr *)data;
	len = skb->len - sizeof(struct udphdr);
	ip = ip_hdr(skb);

	switch (ah->code)
	{
	case 2:
		aun_incoming(skb, ah, len);
		break;
	case 3:
		aun_tx_ack(ah->handle, ECTYPE_TRANSMIT_OK);
		break;
	case 4:
		aun_tx_ack(ah->handle, ECTYPE_TRANSMIT_NOT_LISTENING);
		break;
#if 0
		/* This isn't quite right yet. */
	case 5:
		aun_send_response(ip->saddr, ah->handle, 6, ah->cb);
		break;
#endif
	default:
		printk(KERN_DEBUG "unknown AUN packet (type %d)\n", data[0]);
	}

	skb_free_datagram(sk, skb);
}