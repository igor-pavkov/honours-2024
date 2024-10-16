static const struct xfrm_policy_afinfo *xfrm_policy_get_afinfo(unsigned short family)
{
	const struct xfrm_policy_afinfo *afinfo;

	if (unlikely(family >= ARRAY_SIZE(xfrm_policy_afinfo)))
		return NULL;
	rcu_read_lock();
	afinfo = rcu_dereference(xfrm_policy_afinfo[family]);
	if (unlikely(!afinfo))
		rcu_read_unlock();
	return afinfo;
}