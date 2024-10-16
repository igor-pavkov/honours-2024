static int nf_tables_set_desc_parse(struct nft_set_desc *desc,
				    const struct nlattr *nla)
{
	struct nlattr *da[NFTA_SET_DESC_MAX + 1];
	int err;

	err = nla_parse_nested_deprecated(da, NFTA_SET_DESC_MAX, nla,
					  nft_set_desc_policy, NULL);
	if (err < 0)
		return err;

	if (da[NFTA_SET_DESC_SIZE] != NULL)
		desc->size = ntohl(nla_get_be32(da[NFTA_SET_DESC_SIZE]));
	if (da[NFTA_SET_DESC_CONCAT])
		err = nft_set_desc_concat(desc, da[NFTA_SET_DESC_CONCAT]);

	return err;
}