static void nf_tables_unbind_set(const struct nft_ctx *ctx, struct nft_set *set,
				 struct nft_set_binding *binding, bool event)
{
	list_del_rcu(&binding->list);

	if (list_empty(&set->bindings) && nft_set_is_anonymous(set)) {
		list_del_rcu(&set->list);
		if (event)
			nf_tables_set_notify(ctx, set, NFT_MSG_DELSET,
					     GFP_KERNEL);
	}
}