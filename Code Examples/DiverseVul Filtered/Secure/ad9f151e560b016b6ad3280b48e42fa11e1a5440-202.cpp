static int nf_tables_delchain(struct sk_buff *skb, const struct nfnl_info *info,
			      const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(info->nlh);
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_next(info->net);
	int family = nfmsg->nfgen_family;
	struct net *net = info->net;
	const struct nlattr *attr;
	struct nft_table *table;
	struct nft_chain *chain;
	struct nft_rule *rule;
	struct nft_ctx ctx;
	u64 handle;
	u32 use;
	int err;

	table = nft_table_lookup(net, nla[NFTA_CHAIN_TABLE], family, genmask,
				 NETLINK_CB(skb).portid);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_TABLE]);
		return PTR_ERR(table);
	}

	if (nla[NFTA_CHAIN_HANDLE]) {
		attr = nla[NFTA_CHAIN_HANDLE];
		handle = be64_to_cpu(nla_get_be64(attr));
		chain = nft_chain_lookup_byhandle(table, handle, genmask);
	} else {
		attr = nla[NFTA_CHAIN_NAME];
		chain = nft_chain_lookup(net, table, attr, genmask);
	}
	if (IS_ERR(chain)) {
		NL_SET_BAD_ATTR(extack, attr);
		return PTR_ERR(chain);
	}

	if (info->nlh->nlmsg_flags & NLM_F_NONREC &&
	    chain->use > 0)
		return -EBUSY;

	nft_ctx_init(&ctx, net, skb, info->nlh, family, table, chain, nla);

	use = chain->use;
	list_for_each_entry(rule, &chain->rules, list) {
		if (!nft_is_active_next(net, rule))
			continue;
		use--;

		err = nft_delrule(&ctx, rule);
		if (err < 0)
			return err;
	}

	/* There are rules and elements that are still holding references to us,
	 * we cannot do a recursive removal in this case.
	 */
	if (use > 0) {
		NL_SET_BAD_ATTR(extack, attr);
		return -EBUSY;
	}

	return nft_delchain(&ctx);
}