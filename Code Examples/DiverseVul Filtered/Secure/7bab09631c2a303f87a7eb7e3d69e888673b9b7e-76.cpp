static inline int xfrm_init_path(struct xfrm_dst *path, struct dst_entry *dst,
				 int nfheader_len)
{
	const struct xfrm_policy_afinfo *afinfo =
		xfrm_policy_get_afinfo(dst->ops->family);
	int err;

	if (!afinfo)
		return -EINVAL;

	err = afinfo->init_path(path, dst, nfheader_len);

	rcu_read_unlock();

	return err;
}