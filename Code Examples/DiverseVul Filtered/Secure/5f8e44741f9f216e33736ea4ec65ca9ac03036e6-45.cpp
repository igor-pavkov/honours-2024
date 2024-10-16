void rtmsg_ifinfo(int type, struct net_device *dev, unsigned int change,
		  gfp_t flags)
{
	struct sk_buff *skb;

	if (dev->reg_state != NETREG_REGISTERED)
		return;

	skb = rtmsg_ifinfo_build_skb(type, dev, change, flags);
	if (skb)
		rtmsg_ifinfo_send(skb, dev, flags);
}