static int nf_tables_fill_set_concat(struct sk_buff *skb,
				     const struct nft_set *set)
{
	struct nlattr *concat, *field;
	int i;

	concat = nla_nest_start_noflag(skb, NFTA_SET_DESC_CONCAT);
	if (!concat)
		return -ENOMEM;

	for (i = 0; i < set->field_count; i++) {
		field = nla_nest_start_noflag(skb, NFTA_LIST_ELEM);
		if (!field)
			return -ENOMEM;

		if (nla_put_be32(skb, NFTA_SET_FIELD_LEN,
				 htonl(set->field_len[i])))
			return -ENOMEM;

		nla_nest_end(skb, field);
	}

	nla_nest_end(skb, concat);

	return 0;
}