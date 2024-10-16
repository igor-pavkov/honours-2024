int nft_flow_rule_offload_commit(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_trans *trans;
	int err = 0;
	u8 policy;

	list_for_each_entry(trans, &nft_net->commit_list, list) {
		if (trans->ctx.family != NFPROTO_NETDEV)
			continue;

		switch (trans->msg_type) {
		case NFT_MSG_NEWCHAIN:
			if (!(trans->ctx.chain->flags & NFT_CHAIN_HW_OFFLOAD) ||
			    nft_trans_chain_update(trans))
				continue;

			policy = nft_trans_chain_policy(trans);
			err = nft_flow_offload_chain(trans->ctx.chain, &policy,
						     FLOW_BLOCK_BIND);
			break;
		case NFT_MSG_DELCHAIN:
			if (!(trans->ctx.chain->flags & NFT_CHAIN_HW_OFFLOAD))
				continue;

			policy = nft_trans_chain_policy(trans);
			err = nft_flow_offload_chain(trans->ctx.chain, &policy,
						     FLOW_BLOCK_UNBIND);
			break;
		case NFT_MSG_NEWRULE:
			if (!(trans->ctx.chain->flags & NFT_CHAIN_HW_OFFLOAD))
				continue;

			if (trans->ctx.flags & NLM_F_REPLACE ||
			    !(trans->ctx.flags & NLM_F_APPEND)) {
				err = -EOPNOTSUPP;
				break;
			}
			err = nft_flow_offload_rule(trans->ctx.chain,
						    nft_trans_rule(trans),
						    nft_trans_flow_rule(trans),
						    FLOW_CLS_REPLACE);
			break;
		case NFT_MSG_DELRULE:
			if (!(trans->ctx.chain->flags & NFT_CHAIN_HW_OFFLOAD))
				continue;

			err = nft_flow_offload_rule(trans->ctx.chain,
						    nft_trans_rule(trans),
						    NULL, FLOW_CLS_DESTROY);
			break;
		}

		if (err) {
			nft_flow_rule_offload_abort(net, trans);
			break;
		}
	}

	return err;
}