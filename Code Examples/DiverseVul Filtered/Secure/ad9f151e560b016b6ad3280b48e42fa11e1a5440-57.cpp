static int nft_expr_type_request_module(struct net *net, u8 family,
					struct nlattr *nla)
{
	if (nft_request_module(net, "nft-expr-%u-%.*s", family,
			       nla_len(nla), (char *)nla_data(nla)) == -EAGAIN)
		return -EAGAIN;

	return 0;
}