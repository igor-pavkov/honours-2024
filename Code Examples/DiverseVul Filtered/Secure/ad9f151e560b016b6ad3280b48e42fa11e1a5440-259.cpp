static int nft_delflowtable_hook(struct nft_ctx *ctx,
				 struct nft_flowtable *flowtable)
{
	const struct nlattr * const *nla = ctx->nla;
	struct nft_flowtable_hook flowtable_hook;
	struct nft_hook *this, *hook;
	struct nft_trans *trans;
	int err;

	err = nft_flowtable_parse_hook(ctx, nla[NFTA_FLOWTABLE_HOOK],
				       &flowtable_hook, flowtable, false);
	if (err < 0)
		return err;

	list_for_each_entry(this, &flowtable_hook.list, list) {
		hook = nft_hook_list_find(&flowtable->hook_list, this);
		if (!hook) {
			err = -ENOENT;
			goto err_flowtable_del_hook;
		}
		hook->inactive = true;
	}

	trans = nft_trans_alloc(ctx, NFT_MSG_DELFLOWTABLE,
				sizeof(struct nft_trans_flowtable));
	if (!trans) {
		err = -ENOMEM;
		goto err_flowtable_del_hook;
	}

	nft_trans_flowtable(trans) = flowtable;
	nft_trans_flowtable_update(trans) = true;
	INIT_LIST_HEAD(&nft_trans_flowtable_hooks(trans));
	nft_flowtable_hook_release(&flowtable_hook);

	nft_trans_commit_list_add_tail(ctx->net, trans);

	return 0;

err_flowtable_del_hook:
	list_for_each_entry(this, &flowtable_hook.list, list) {
		hook = nft_hook_list_find(&flowtable->hook_list, this);
		if (!hook)
			break;

		hook->inactive = false;
	}
	nft_flowtable_hook_release(&flowtable_hook);

	return err;
}