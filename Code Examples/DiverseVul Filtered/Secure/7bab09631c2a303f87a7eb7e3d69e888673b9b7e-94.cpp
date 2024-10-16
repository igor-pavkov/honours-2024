int __xfrm_decode_session(struct sk_buff *skb, struct flowi *fl,
			  unsigned int family, int reverse)
{
	const struct xfrm_policy_afinfo *afinfo = xfrm_policy_get_afinfo(family);
	int err;

	if (unlikely(afinfo == NULL))
		return -EAFNOSUPPORT;

	afinfo->decode_session(skb, fl, reverse);
	err = security_xfrm_decode_session(skb, &fl->flowi_secid);
	rcu_read_unlock();
	return err;
}