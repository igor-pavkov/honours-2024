static const struct rtnl_link_ops *linkinfo_to_kind_ops(const struct nlattr *nla)
{
	const struct rtnl_link_ops *ops = NULL;
	struct nlattr *linfo[IFLA_INFO_MAX + 1];

	if (nla_parse_nested(linfo, IFLA_INFO_MAX, nla, ifla_info_policy) < 0)
		return NULL;

	if (linfo[IFLA_INFO_KIND]) {
		char kind[MODULE_NAME_LEN];

		nla_strlcpy(kind, linfo[IFLA_INFO_KIND], sizeof(kind));
		ops = rtnl_link_ops_get(kind);
	}

	return ops;
}