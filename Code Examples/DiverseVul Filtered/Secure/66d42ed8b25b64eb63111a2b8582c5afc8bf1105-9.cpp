static __be16 ppp_type_trans(struct sk_buff *skb, struct net_device *dev)
{
	struct hdlc_header *data = (struct hdlc_header*)skb->data;

	if (skb->len < sizeof(struct hdlc_header))
		return htons(ETH_P_HDLC);
	if (data->address != HDLC_ADDR_ALLSTATIONS ||
	    data->control != HDLC_CTRL_UI)
		return htons(ETH_P_HDLC);

	switch (data->protocol) {
	case cpu_to_be16(PID_IP):
		skb_pull(skb, sizeof(struct hdlc_header));
		return htons(ETH_P_IP);

	case cpu_to_be16(PID_IPV6):
		skb_pull(skb, sizeof(struct hdlc_header));
		return htons(ETH_P_IPV6);

	default:
		return htons(ETH_P_HDLC);
	}
}