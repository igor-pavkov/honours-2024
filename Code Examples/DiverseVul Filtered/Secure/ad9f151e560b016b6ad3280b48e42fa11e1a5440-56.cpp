static int nf_tables_updchain(struct nft_ctx *ctx, u8 genmask, u8 policy,
			      u32 flags, const struct nlattr *attr,
			      struct netlink_ext_ack *extack)
{
	const struct nlattr * const *nla = ctx->nla;
	struct nft_table *table = ctx->table;
	struct nft_chain *chain = ctx->chain;
	struct nft_base_chain *basechain;
	struct nft_stats *stats = NULL;
	struct nft_chain_hook hook;
	struct nf_hook_ops *ops;
	struct nft_trans *trans;
	int err;

	if (chain->flags ^ flags)
		return -EOPNOTSUPP;

	if (nla[NFTA_CHAIN_HOOK]) {
		if (!nft_is_base_chain(chain)) {
			NL_SET_BAD_ATTR(extack, attr);
			return -EEXIST;
		}
		err = nft_chain_parse_hook(ctx->net, nla, &hook, ctx->family,
					   extack, false);
		if (err < 0)
			return err;

		basechain = nft_base_chain(chain);
		if (basechain->type != hook.type) {
			nft_chain_release_hook(&hook);
			NL_SET_BAD_ATTR(extack, attr);
			return -EEXIST;
		}

		if (nft_base_chain_netdev(ctx->family, hook.num)) {
			if (!nft_hook_list_equal(&basechain->hook_list,
						 &hook.list)) {
				nft_chain_release_hook(&hook);
				NL_SET_BAD_ATTR(extack, attr);
				return -EEXIST;
			}
		} else {
			ops = &basechain->ops;
			if (ops->hooknum != hook.num ||
			    ops->priority != hook.priority) {
				nft_chain_release_hook(&hook);
				NL_SET_BAD_ATTR(extack, attr);
				return -EEXIST;
			}
		}
		nft_chain_release_hook(&hook);
	}

	if (nla[NFTA_CHAIN_HANDLE] &&
	    nla[NFTA_CHAIN_NAME]) {
		struct nft_chain *chain2;

		chain2 = nft_chain_lookup(ctx->net, table,
					  nla[NFTA_CHAIN_NAME], genmask);
		if (!IS_ERR(chain2)) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_NAME]);
			return -EEXIST;
		}
	}

	if (nla[NFTA_CHAIN_COUNTERS]) {
		if (!nft_is_base_chain(chain))
			return -EOPNOTSUPP;

		stats = nft_stats_alloc(nla[NFTA_CHAIN_COUNTERS]);
		if (IS_ERR(stats))
			return PTR_ERR(stats);
	}

	err = -ENOMEM;
	trans = nft_trans_alloc(ctx, NFT_MSG_NEWCHAIN,
				sizeof(struct nft_trans_chain));
	if (trans == NULL)
		goto err;

	nft_trans_chain_stats(trans) = stats;
	nft_trans_chain_update(trans) = true;

	if (nla[NFTA_CHAIN_POLICY])
		nft_trans_chain_policy(trans) = policy;
	else
		nft_trans_chain_policy(trans) = -1;

	if (nla[NFTA_CHAIN_HANDLE] &&
	    nla[NFTA_CHAIN_NAME]) {
		struct nftables_pernet *nft_net = nft_pernet(ctx->net);
		struct nft_trans *tmp;
		char *name;

		err = -ENOMEM;
		name = nla_strdup(nla[NFTA_CHAIN_NAME], GFP_KERNEL);
		if (!name)
			goto err;

		err = -EEXIST;
		list_for_each_entry(tmp, &nft_net->commit_list, list) {
			if (tmp->msg_type == NFT_MSG_NEWCHAIN &&
			    tmp->ctx.table == table &&
			    nft_trans_chain_update(tmp) &&
			    nft_trans_chain_name(tmp) &&
			    strcmp(name, nft_trans_chain_name(tmp)) == 0) {
				NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_NAME]);
				kfree(name);
				goto err;
			}
		}

		nft_trans_chain_name(trans) = name;
	}
	nft_trans_commit_list_add_tail(ctx->net, trans);

	return 0;
err:
	free_percpu(stats);
	kfree(trans);
	return err;
}