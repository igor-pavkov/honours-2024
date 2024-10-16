static inline int xfrm_fill_dst(struct xfrm_dst *xdst, struct net_device *dev,
				const struct flowi *fl)
{
	const struct xfrm_policy_afinfo *afinfo =
		xfrm_policy_get_afinfo(xdst->u.dst.ops->family);
	int err;

	if (!afinfo)
		return -EINVAL;

	err = afinfo->fill_dst(xdst, dev, fl);

	rcu_read_unlock();

	return err;
}