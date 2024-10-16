static void xfrm_netlink_rcv(struct sk_buff *skb)
{
	mutex_lock(&xfrm_cfg_mutex);
	netlink_rcv_skb(skb, &xfrm_user_rcv_msg);
	mutex_unlock(&xfrm_cfg_mutex);
}