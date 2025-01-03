static inline int xfrm_nlmsg_multicast(struct net *net, struct sk_buff *skb,
				       u32 pid, unsigned int group)
{
	struct sock *nlsk = rcu_dereference(net->xfrm.nlsk);

	if (nlsk)
		return nlmsg_multicast(nlsk, skb, pid, group, GFP_ATOMIC);
	else
		return -1;
}