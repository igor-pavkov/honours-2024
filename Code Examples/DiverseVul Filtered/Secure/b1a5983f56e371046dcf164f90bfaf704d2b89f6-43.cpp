static void nft_flow_rule_offload_abort(struct net *net,
					struct nft_trans *trans)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	int err = 0;

	list_for_each_entry_continue_reverse(trans, &nft_net->commit_list, list) {
		if (trans->ctx.family != NFPROTO_NETDEV)
			continue;

		switch (trans->msg_type) {
		case NFT_MSG_NEWCHAIN:
			if (!(trans->ctx.chain->flags & NFT_CHAIN_HW_OFFLOAD) ||
			    nft_trans_chain_update(trans))
				continue;

			err = nft_flow_offload_chain(trans->ctx.chain, NULL,
						     FLOW_BLOCK_UNBIND);
			break;
		case NFT_MSG_DELCHAIN:
			if (!(trans->ctx.chain->flags & NFT_CHAIN_HW_OFFLOAD))
				continue;

			err = nft_flow_offload_chain(trans->ctx.chain, NULL,
						     FLOW_BLOCK_BIND);
			break;
		case NFT_MSG_NEWRULE:
			if (!(trans->ctx.chain->flags & NFT_CHAIN_HW_OFFLOAD))
				continue;

			err = nft_flow_offload_rule(trans->ctx.chain,
						    nft_trans_rule(trans),
						    NULL, FLOW_CLS_DESTROY);
			break;
		case NFT_MSG_DELRULE:
			if (!(trans->ctx.chain->flags & NFT_CHAIN_HW_OFFLOAD))
				continue;

			err = nft_flow_offload_rule(trans->ctx.chain,
						    nft_trans_rule(trans),
						    nft_trans_flow_rule(trans),
						    FLOW_CLS_REPLACE);
			break;
		}

		if (WARN_ON_ONCE(err))
			break;
	}
}