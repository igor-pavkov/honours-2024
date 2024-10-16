static int packet_direct_xmit(struct sk_buff *skb)
{
#ifdef CONFIG_NETFILTER_EGRESS
	if (nf_hook_egress_active()) {
		skb = nf_hook_direct_egress(skb);
		if (!skb)
			return NET_XMIT_DROP;
	}
#endif
	return dev_direct_xmit(skb, packet_pick_tx_queue(skb));
}