static int nft_chain_parse_hook(struct net *net,
				const struct nlattr * const nla[],
				struct nft_chain_hook *hook, u8 family,
				struct netlink_ext_ack *extack, bool autoload)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nlattr *ha[NFTA_HOOK_MAX + 1];
	const struct nft_chain_type *type;
	int err;

	lockdep_assert_held(&nft_net->commit_mutex);
	lockdep_nfnl_nft_mutex_not_held();

	err = nla_parse_nested_deprecated(ha, NFTA_HOOK_MAX,
					  nla[NFTA_CHAIN_HOOK],
					  nft_hook_policy, NULL);
	if (err < 0)
		return err;

	if (ha[NFTA_HOOK_HOOKNUM] == NULL ||
	    ha[NFTA_HOOK_PRIORITY] == NULL)
		return -EINVAL;

	hook->num = ntohl(nla_get_be32(ha[NFTA_HOOK_HOOKNUM]));
	hook->priority = ntohl(nla_get_be32(ha[NFTA_HOOK_PRIORITY]));

	type = __nft_chain_type_get(family, NFT_CHAIN_T_DEFAULT);
	if (!type)
		return -EOPNOTSUPP;

	if (nla[NFTA_CHAIN_TYPE]) {
		type = nf_tables_chain_type_lookup(net, nla[NFTA_CHAIN_TYPE],
						   family, autoload);
		if (IS_ERR(type)) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_TYPE]);
			return PTR_ERR(type);
		}
	}
	if (hook->num >= NFT_MAX_HOOKS || !(type->hook_mask & (1 << hook->num)))
		return -EOPNOTSUPP;

	if (type->type == NFT_CHAIN_T_NAT &&
	    hook->priority <= NF_IP_PRI_CONNTRACK)
		return -EOPNOTSUPP;

	if (!try_module_get(type->owner)) {
		if (nla[NFTA_CHAIN_TYPE])
			NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_TYPE]);
		return -ENOENT;
	}

	hook->type = type;

	INIT_LIST_HEAD(&hook->list);
	if (nft_base_chain_netdev(family, hook->num)) {
		err = nft_chain_parse_netdev(net, ha, &hook->list);
		if (err < 0) {
			module_put(type->owner);
			return err;
		}
	} else if (ha[NFTA_HOOK_DEV] || ha[NFTA_HOOK_DEVS]) {
		module_put(type->owner);
		return -EOPNOTSUPP;
	}

	return 0;
}