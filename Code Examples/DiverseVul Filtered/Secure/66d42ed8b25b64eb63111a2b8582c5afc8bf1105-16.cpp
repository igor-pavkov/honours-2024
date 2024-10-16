static int ppp_hard_header(struct sk_buff *skb, struct net_device *dev,
			   u16 type, const void *daddr, const void *saddr,
			   unsigned int len)
{
	struct hdlc_header *data;
#if DEBUG_HARD_HEADER
	printk(KERN_DEBUG "%s: ppp_hard_header() called\n", dev->name);
#endif

	skb_push(skb, sizeof(struct hdlc_header));
	data = (struct hdlc_header*)skb->data;

	data->address = HDLC_ADDR_ALLSTATIONS;
	data->control = HDLC_CTRL_UI;
	switch (type) {
	case ETH_P_IP:
		data->protocol = htons(PID_IP);
		break;
	case ETH_P_IPV6:
		data->protocol = htons(PID_IPV6);
		break;
	case PID_LCP:
	case PID_IPCP:
	case PID_IPV6CP:
		data->protocol = htons(type);
		break;
	default:		/* unknown protocol */
		data->protocol = 0;
	}
	return sizeof(struct hdlc_header);
}