static int nft_trans_obj_add(struct nft_ctx *ctx, int msg_type,
			     struct nft_object *obj)
{
	struct nft_trans *trans;

	trans = nft_trans_alloc(ctx, msg_type, sizeof(struct nft_trans_obj));
	if (trans == NULL)
		return -ENOMEM;

	if (msg_type == NFT_MSG_NEWOBJ)
		nft_activate_next(ctx->net, obj);

	nft_trans_obj(trans) = obj;
	nft_trans_commit_list_add_tail(ctx->net, trans);

	return 0;
}