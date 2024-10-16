static int nf_tables_fill_table_info(struct sk_buff *skb, struct net *net,
				     u32 portid, u32 seq, int event, u32 flags,
				     int family, const struct nft_table *table)
{
	struct nlmsghdr *nlh;

	event = nfnl_msg_type(NFNL_SUBSYS_NFTABLES, event);
	nlh = nfnl_msg_put(skb, portid, seq, event, flags, family,
			   NFNETLINK_V0, nft_base_seq(net));
	if (!nlh)
		goto nla_put_failure;

	if (nla_put_string(skb, NFTA_TABLE_NAME, table->name) ||
	    nla_put_be32(skb, NFTA_TABLE_FLAGS,
			 htonl(table->flags & NFT_TABLE_F_MASK)) ||
	    nla_put_be32(skb, NFTA_TABLE_USE, htonl(table->use)) ||
	    nla_put_be64(skb, NFTA_TABLE_HANDLE, cpu_to_be64(table->handle),
			 NFTA_TABLE_PAD))
		goto nla_put_failure;
	if (nft_table_has_owner(table) &&
	    nla_put_be32(skb, NFTA_TABLE_OWNER, htonl(table->nlpid)))
		goto nla_put_failure;

	if (table->udata) {
		if (nla_put(skb, NFTA_TABLE_USERDATA, table->udlen, table->udata))
			goto nla_put_failure;
	}

	nlmsg_end(skb, nlh);
	return 0;

nla_put_failure:
	nlmsg_trim(skb, nlh);
	return -1;
}