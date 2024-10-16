static int copy_to_user_policy_type(u8 type, struct sk_buff *skb)
{
	struct xfrm_userpolicy_type upt;

	/* Sadly there are two holes in struct xfrm_userpolicy_type */
	memset(&upt, 0, sizeof(upt));
	upt.type = type;

	return nla_put(skb, XFRMA_POLICY_TYPE, sizeof(upt), &upt);
}