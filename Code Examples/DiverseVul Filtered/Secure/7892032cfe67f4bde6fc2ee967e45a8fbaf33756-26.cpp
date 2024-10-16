static int ip6gre_tap_validate(struct nlattr *tb[], struct nlattr *data[])
{
	struct in6_addr daddr;

	if (tb[IFLA_ADDRESS]) {
		if (nla_len(tb[IFLA_ADDRESS]) != ETH_ALEN)
			return -EINVAL;
		if (!is_valid_ether_addr(nla_data(tb[IFLA_ADDRESS])))
			return -EADDRNOTAVAIL;
	}

	if (!data)
		goto out;

	if (data[IFLA_GRE_REMOTE]) {
		daddr = nla_get_in6_addr(data[IFLA_GRE_REMOTE]);
		if (ipv6_addr_any(&daddr))
			return -EINVAL;
	}

out:
	return ip6gre_tunnel_validate(tb, data);
}