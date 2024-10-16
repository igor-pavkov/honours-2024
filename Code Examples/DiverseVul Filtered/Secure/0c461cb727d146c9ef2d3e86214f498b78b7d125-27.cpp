static int selinux_netlink_send(struct sock *sk, struct sk_buff *skb)
{
	return selinux_nlmsg_perm(sk, skb);
}