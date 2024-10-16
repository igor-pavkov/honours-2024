static int ieee802154_rcv(struct sk_buff *skb, struct net_device *dev,
			  struct packet_type *pt, struct net_device *orig_dev)
{
	if (!netif_running(dev))
		goto drop;
	pr_debug("got frame, type %d, dev %p\n", dev->type, dev);
#ifdef DEBUG
	print_hex_dump_bytes("ieee802154_rcv ",
			     DUMP_PREFIX_NONE, skb->data, skb->len);
#endif

	if (!net_eq(dev_net(dev), &init_net))
		goto drop;

	ieee802154_raw_deliver(dev, skb);

	if (dev->type != ARPHRD_IEEE802154)
		goto drop;

	if (skb->pkt_type != PACKET_OTHERHOST)
		return ieee802154_dgram_deliver(dev, skb);

drop:
	kfree_skb(skb);
	return NET_RX_DROP;
}