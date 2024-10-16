static int copy_user_offload(struct xfrm_state_offload *xso, struct sk_buff *skb)
{
	struct xfrm_user_offload *xuo;
	struct nlattr *attr;

	attr = nla_reserve(skb, XFRMA_OFFLOAD_DEV, sizeof(*xuo));
	if (attr == NULL)
		return -EMSGSIZE;

	xuo = nla_data(attr);
	memset(xuo, 0, sizeof(*xuo));
	xuo->ifindex = xso->dev->ifindex;
	xuo->flags = xso->flags;

	return 0;
}