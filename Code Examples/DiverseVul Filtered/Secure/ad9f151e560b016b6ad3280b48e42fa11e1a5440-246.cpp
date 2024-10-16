static void nft_commit_release(struct nft_trans *trans)
{
	switch (trans->msg_type) {
	case NFT_MSG_DELTABLE:
		nf_tables_table_destroy(&trans->ctx);
		break;
	case NFT_MSG_NEWCHAIN:
		free_percpu(nft_trans_chain_stats(trans));
		kfree(nft_trans_chain_name(trans));
		break;
	case NFT_MSG_DELCHAIN:
		nf_tables_chain_destroy(&trans->ctx);
		break;
	case NFT_MSG_DELRULE:
		nf_tables_rule_destroy(&trans->ctx, nft_trans_rule(trans));
		break;
	case NFT_MSG_DELSET:
		nft_set_destroy(&trans->ctx, nft_trans_set(trans));
		break;
	case NFT_MSG_DELSETELEM:
		nf_tables_set_elem_destroy(&trans->ctx,
					   nft_trans_elem_set(trans),
					   nft_trans_elem(trans).priv);
		break;
	case NFT_MSG_DELOBJ:
		nft_obj_destroy(&trans->ctx, nft_trans_obj(trans));
		break;
	case NFT_MSG_DELFLOWTABLE:
		if (nft_trans_flowtable_update(trans))
			nft_flowtable_hooks_destroy(&nft_trans_flowtable_hooks(trans));
		else
			nf_tables_flowtable_destroy(nft_trans_flowtable(trans));
		break;
	}

	if (trans->put_net)
		put_net(trans->ctx.net);

	kfree(trans);
}