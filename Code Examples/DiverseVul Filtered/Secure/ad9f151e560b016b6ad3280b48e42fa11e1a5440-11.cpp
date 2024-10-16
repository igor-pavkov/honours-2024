static int nft_setelem_flush(const struct nft_ctx *ctx,
			     struct nft_set *set,
			     const struct nft_set_iter *iter,
			     struct nft_set_elem *elem)
{
	struct nft_trans *trans;
	int err;

	trans = nft_trans_alloc_gfp(ctx, NFT_MSG_DELSETELEM,
				    sizeof(struct nft_trans_elem), GFP_ATOMIC);
	if (!trans)
		return -ENOMEM;

	if (!set->ops->flush(ctx->net, set, elem->priv)) {
		err = -ENOENT;
		goto err1;
	}
	set->ndeact++;

	nft_setelem_data_deactivate(ctx->net, set, elem);
	nft_trans_elem_set(trans) = set;
	nft_trans_elem(trans) = *elem;
	nft_trans_commit_list_add_tail(ctx->net, trans);

	return 0;
err1:
	kfree(trans);
	return err;
}