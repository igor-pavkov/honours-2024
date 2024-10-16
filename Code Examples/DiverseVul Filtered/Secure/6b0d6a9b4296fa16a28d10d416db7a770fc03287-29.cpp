int br_multicast_rcv(struct net_bridge *br, struct net_bridge_port *port,
		     struct sk_buff *skb)
{
	BR_INPUT_SKB_CB(skb)->igmp = 0;
	BR_INPUT_SKB_CB(skb)->mrouters_only = 0;

	if (br->multicast_disabled)
		return 0;

	switch (skb->protocol) {
	case htons(ETH_P_IP):
		return br_multicast_ipv4_rcv(br, port, skb);
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	case htons(ETH_P_IPV6):
		return br_multicast_ipv6_rcv(br, port, skb);
#endif
	}

	return 0;
}