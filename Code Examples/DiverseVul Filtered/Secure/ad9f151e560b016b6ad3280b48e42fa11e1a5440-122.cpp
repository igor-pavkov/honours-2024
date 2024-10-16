static struct nft_rule *nft_rule_lookup(const struct nft_chain *chain,
					const struct nlattr *nla)
{
	if (nla == NULL)
		return ERR_PTR(-EINVAL);

	return __nft_rule_lookup(chain, be64_to_cpu(nla_get_be64(nla)));
}