static int nft_flowtable_parse_hook(const struct nft_ctx *ctx,
				    const struct nlattr *attr,
				    struct nft_flowtable_hook *flowtable_hook,
				    struct nft_flowtable *flowtable, bool add)
{
	struct nlattr *tb[NFTA_FLOWTABLE_HOOK_MAX + 1];
	struct nft_hook *hook;
	int hooknum, priority;
	int err;

	INIT_LIST_HEAD(&flowtable_hook->list);

	err = nla_parse_nested_deprecated(tb, NFTA_FLOWTABLE_HOOK_MAX, attr,
					  nft_flowtable_hook_policy, NULL);
	if (err < 0)
		return err;

	if (add) {
		if (!tb[NFTA_FLOWTABLE_HOOK_NUM] ||
		    !tb[NFTA_FLOWTABLE_HOOK_PRIORITY])
			return -EINVAL;

		hooknum = ntohl(nla_get_be32(tb[NFTA_FLOWTABLE_HOOK_NUM]));
		if (hooknum != NF_NETDEV_INGRESS)
			return -EOPNOTSUPP;

		priority = ntohl(nla_get_be32(tb[NFTA_FLOWTABLE_HOOK_PRIORITY]));

		flowtable_hook->priority	= priority;
		flowtable_hook->num		= hooknum;
	} else {
		if (tb[NFTA_FLOWTABLE_HOOK_NUM]) {
			hooknum = ntohl(nla_get_be32(tb[NFTA_FLOWTABLE_HOOK_NUM]));
			if (hooknum != flowtable->hooknum)
				return -EOPNOTSUPP;
		}

		if (tb[NFTA_FLOWTABLE_HOOK_PRIORITY]) {
			priority = ntohl(nla_get_be32(tb[NFTA_FLOWTABLE_HOOK_PRIORITY]));
			if (priority != flowtable->data.priority)
				return -EOPNOTSUPP;
		}

		flowtable_hook->priority	= flowtable->data.priority;
		flowtable_hook->num		= flowtable->hooknum;
	}

	if (tb[NFTA_FLOWTABLE_HOOK_DEVS]) {
		err = nf_tables_parse_netdev_hooks(ctx->net,
						   tb[NFTA_FLOWTABLE_HOOK_DEVS],
						   &flowtable_hook->list);
		if (err < 0)
			return err;
	}

	list_for_each_entry(hook, &flowtable_hook->list, list) {
		hook->ops.pf		= NFPROTO_NETDEV;
		hook->ops.hooknum	= flowtable_hook->num;
		hook->ops.priority	= flowtable_hook->priority;
		hook->ops.priv		= &flowtable->data;
		hook->ops.hook		= flowtable->data.type->hook;
	}

	return err;
}