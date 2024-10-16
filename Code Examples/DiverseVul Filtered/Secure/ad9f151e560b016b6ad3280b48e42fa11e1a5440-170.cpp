static int nft_flowtable_update(struct nft_ctx *ctx, const struct nlmsghdr *nlh,
				struct nft_flowtable *flowtable)
{
	const struct nlattr * const *nla = ctx->nla;
	struct nft_flowtable_hook flowtable_hook;
	struct nft_hook *hook, *next;
	struct nft_trans *trans;
	bool unregister = false;
	u32 flags;
	int err;

	err = nft_flowtable_parse_hook(ctx, nla[NFTA_FLOWTABLE_HOOK],
				       &flowtable_hook, flowtable, false);
	if (err < 0)
		return err;

	list_for_each_entry_safe(hook, next, &flowtable_hook.list, list) {
		if (nft_hook_list_find(&flowtable->hook_list, hook)) {
			list_del(&hook->list);
			kfree(hook);
		}
	}

	if (nla[NFTA_FLOWTABLE_FLAGS]) {
		flags = ntohl(nla_get_be32(nla[NFTA_FLOWTABLE_FLAGS]));
		if (flags & ~NFT_FLOWTABLE_MASK)
			return -EOPNOTSUPP;
		if ((flowtable->data.flags & NFT_FLOWTABLE_HW_OFFLOAD) ^
		    (flags & NFT_FLOWTABLE_HW_OFFLOAD))
			return -EOPNOTSUPP;
	} else {
		flags = flowtable->data.flags;
	}

	err = nft_register_flowtable_net_hooks(ctx->net, ctx->table,
					       &flowtable_hook.list, flowtable);
	if (err < 0)
		goto err_flowtable_update_hook;

	trans = nft_trans_alloc(ctx, NFT_MSG_NEWFLOWTABLE,
				sizeof(struct nft_trans_flowtable));
	if (!trans) {
		unregister = true;
		err = -ENOMEM;
		goto err_flowtable_update_hook;
	}

	nft_trans_flowtable_flags(trans) = flags;
	nft_trans_flowtable(trans) = flowtable;
	nft_trans_flowtable_update(trans) = true;
	INIT_LIST_HEAD(&nft_trans_flowtable_hooks(trans));
	list_splice(&flowtable_hook.list, &nft_trans_flowtable_hooks(trans));

	nft_trans_commit_list_add_tail(ctx->net, trans);

	return 0;

err_flowtable_update_hook:
	list_for_each_entry_safe(hook, next, &flowtable_hook.list, list) {
		if (unregister)
			nft_unregister_flowtable_hook(ctx->net, flowtable, hook);
		list_del_rcu(&hook->list);
		kfree_rcu(hook, rcu);
	}

	return err;

}