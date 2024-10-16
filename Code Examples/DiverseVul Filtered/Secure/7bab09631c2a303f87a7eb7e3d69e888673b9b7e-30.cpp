int xfrm_policy_register_afinfo(const struct xfrm_policy_afinfo *afinfo, int family)
{
	int err = 0;

	if (WARN_ON(family >= ARRAY_SIZE(xfrm_policy_afinfo)))
		return -EAFNOSUPPORT;

	spin_lock(&xfrm_policy_afinfo_lock);
	if (unlikely(xfrm_policy_afinfo[family] != NULL))
		err = -EEXIST;
	else {
		struct dst_ops *dst_ops = afinfo->dst_ops;
		if (likely(dst_ops->kmem_cachep == NULL))
			dst_ops->kmem_cachep = xfrm_dst_cache;
		if (likely(dst_ops->check == NULL))
			dst_ops->check = xfrm_dst_check;
		if (likely(dst_ops->default_advmss == NULL))
			dst_ops->default_advmss = xfrm_default_advmss;
		if (likely(dst_ops->mtu == NULL))
			dst_ops->mtu = xfrm_mtu;
		if (likely(dst_ops->negative_advice == NULL))
			dst_ops->negative_advice = xfrm_negative_advice;
		if (likely(dst_ops->link_failure == NULL))
			dst_ops->link_failure = xfrm_link_failure;
		if (likely(dst_ops->neigh_lookup == NULL))
			dst_ops->neigh_lookup = xfrm_neigh_lookup;
		if (likely(!dst_ops->confirm_neigh))
			dst_ops->confirm_neigh = xfrm_confirm_neigh;
		rcu_assign_pointer(xfrm_policy_afinfo[family], afinfo);
	}
	spin_unlock(&xfrm_policy_afinfo_lock);

	return err;
}