static int econet_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
{
	struct ec_framehdr *hdr;
	struct sock *sk;
	struct ec_device *edev = dev->ec_ptr;

	if (!net_eq(dev_net(dev), &init_net))
		goto drop;

	if (skb->pkt_type == PACKET_OTHERHOST)
		goto drop;

	if (!edev)
		goto drop;

	if ((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)
		return NET_RX_DROP;

	if (!pskb_may_pull(skb, sizeof(struct ec_framehdr)))
		goto drop;

	hdr = (struct ec_framehdr *) skb->data;

	/* First check for encapsulated IP */
	if (hdr->port == EC_PORT_IP) {
		skb->protocol = htons(ETH_P_IP);
		skb_pull(skb, sizeof(struct ec_framehdr));
		netif_rx(skb);
		return 0;
	}

	sk = ec_listening_socket(hdr->port, hdr->src_stn, hdr->src_net);
	if (!sk)
		goto drop;

	if (ec_queue_packet(sk, skb, edev->net, hdr->src_stn, hdr->cb,
			    hdr->port))
		goto drop;

	return 0;

drop:
	kfree_skb(skb);
	return NET_RX_DROP;
}