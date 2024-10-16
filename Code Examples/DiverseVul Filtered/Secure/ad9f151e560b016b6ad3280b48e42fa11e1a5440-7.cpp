static int nft_dump_basechain_hook(struct sk_buff *skb, int family,
				   const struct nft_base_chain *basechain)
{
	const struct nf_hook_ops *ops = &basechain->ops;
	struct nft_hook *hook, *first = NULL;
	struct nlattr *nest, *nest_devs;
	int n = 0;

	nest = nla_nest_start_noflag(skb, NFTA_CHAIN_HOOK);
	if (nest == NULL)
		goto nla_put_failure;
	if (nla_put_be32(skb, NFTA_HOOK_HOOKNUM, htonl(ops->hooknum)))
		goto nla_put_failure;
	if (nla_put_be32(skb, NFTA_HOOK_PRIORITY, htonl(ops->priority)))
		goto nla_put_failure;

	if (nft_base_chain_netdev(family, ops->hooknum)) {
		nest_devs = nla_nest_start_noflag(skb, NFTA_HOOK_DEVS);
		list_for_each_entry(hook, &basechain->hook_list, list) {
			if (!first)
				first = hook;

			if (nla_put_string(skb, NFTA_DEVICE_NAME,
					   hook->ops.dev->name))
				goto nla_put_failure;
			n++;
		}
		nla_nest_end(skb, nest_devs);

		if (n == 1 &&
		    nla_put_string(skb, NFTA_HOOK_DEV, first->ops.dev->name))
			goto nla_put_failure;
	}
	nla_nest_end(skb, nest);

	return 0;
nla_put_failure:
	return -1;
}