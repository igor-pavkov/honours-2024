static int brport_nla_put_flag(struct sk_buff *skb, u32 flags, u32 mask,
			       unsigned int attrnum, unsigned int flag)
{
	if (mask & flag)
		return nla_put_u8(skb, attrnum, !!(flags & flag));
	return 0;
}