static int nf_tables_fill_set(struct sk_buff *skb, const struct nft_ctx *ctx,
			      const struct nft_set *set, u16 event, u16 flags)
{
	struct nlmsghdr *nlh;
	u32 portid = ctx->portid;
	struct nlattr *nest;
	u32 seq = ctx->seq;
	int i;

	event = nfnl_msg_type(NFNL_SUBSYS_NFTABLES, event);
	nlh = nfnl_msg_put(skb, portid, seq, event, flags, ctx->family,
			   NFNETLINK_V0, nft_base_seq(ctx->net));
	if (!nlh)
		goto nla_put_failure;

	if (nla_put_string(skb, NFTA_SET_TABLE, ctx->table->name))
		goto nla_put_failure;
	if (nla_put_string(skb, NFTA_SET_NAME, set->name))
		goto nla_put_failure;
	if (nla_put_be64(skb, NFTA_SET_HANDLE, cpu_to_be64(set->handle),
			 NFTA_SET_PAD))
		goto nla_put_failure;
	if (set->flags != 0)
		if (nla_put_be32(skb, NFTA_SET_FLAGS, htonl(set->flags)))
			goto nla_put_failure;

	if (nla_put_be32(skb, NFTA_SET_KEY_TYPE, htonl(set->ktype)))
		goto nla_put_failure;
	if (nla_put_be32(skb, NFTA_SET_KEY_LEN, htonl(set->klen)))
		goto nla_put_failure;
	if (set->flags & NFT_SET_MAP) {
		if (nla_put_be32(skb, NFTA_SET_DATA_TYPE, htonl(set->dtype)))
			goto nla_put_failure;
		if (nla_put_be32(skb, NFTA_SET_DATA_LEN, htonl(set->dlen)))
			goto nla_put_failure;
	}
	if (set->flags & NFT_SET_OBJECT &&
	    nla_put_be32(skb, NFTA_SET_OBJ_TYPE, htonl(set->objtype)))
		goto nla_put_failure;

	if (set->timeout &&
	    nla_put_be64(skb, NFTA_SET_TIMEOUT,
			 nf_jiffies64_to_msecs(set->timeout),
			 NFTA_SET_PAD))
		goto nla_put_failure;
	if (set->gc_int &&
	    nla_put_be32(skb, NFTA_SET_GC_INTERVAL, htonl(set->gc_int)))
		goto nla_put_failure;

	if (set->policy != NFT_SET_POL_PERFORMANCE) {
		if (nla_put_be32(skb, NFTA_SET_POLICY, htonl(set->policy)))
			goto nla_put_failure;
	}

	if (set->udata &&
	    nla_put(skb, NFTA_SET_USERDATA, set->udlen, set->udata))
		goto nla_put_failure;

	nest = nla_nest_start_noflag(skb, NFTA_SET_DESC);
	if (!nest)
		goto nla_put_failure;
	if (set->size &&
	    nla_put_be32(skb, NFTA_SET_DESC_SIZE, htonl(set->size)))
		goto nla_put_failure;

	if (set->field_count > 1 &&
	    nf_tables_fill_set_concat(skb, set))
		goto nla_put_failure;

	nla_nest_end(skb, nest);

	if (set->num_exprs == 1) {
		nest = nla_nest_start_noflag(skb, NFTA_SET_EXPR);
		if (nf_tables_fill_expr_info(skb, set->exprs[0]) < 0)
			goto nla_put_failure;

		nla_nest_end(skb, nest);
	} else if (set->num_exprs > 1) {
		nest = nla_nest_start_noflag(skb, NFTA_SET_EXPRESSIONS);
		if (nest == NULL)
			goto nla_put_failure;

		for (i = 0; i < set->num_exprs; i++) {
			if (nft_expr_dump(skb, NFTA_LIST_ELEM,
					  set->exprs[i]) < 0)
				goto nla_put_failure;
		}
		nla_nest_end(skb, nest);
	}

	nlmsg_end(skb, nlh);
	return 0;

nla_put_failure:
	nlmsg_trim(skb, nlh);
	return -1;
}