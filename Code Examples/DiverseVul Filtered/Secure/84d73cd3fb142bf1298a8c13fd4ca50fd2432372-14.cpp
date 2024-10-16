static const struct rtnl_af_ops *rtnl_af_lookup(const int family)
{
	const struct rtnl_af_ops *ops;

	list_for_each_entry(ops, &rtnl_af_ops, list) {
		if (ops->family == family)
			return ops;
	}

	return NULL;
}