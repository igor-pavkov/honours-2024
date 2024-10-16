static bool ip6gre_netlink_encap_parms(struct nlattr *data[],
				       struct ip_tunnel_encap *ipencap)
{
	bool ret = false;

	memset(ipencap, 0, sizeof(*ipencap));

	if (!data)
		return ret;

	if (data[IFLA_GRE_ENCAP_TYPE]) {
		ret = true;
		ipencap->type = nla_get_u16(data[IFLA_GRE_ENCAP_TYPE]);
	}

	if (data[IFLA_GRE_ENCAP_FLAGS]) {
		ret = true;
		ipencap->flags = nla_get_u16(data[IFLA_GRE_ENCAP_FLAGS]);
	}

	if (data[IFLA_GRE_ENCAP_SPORT]) {
		ret = true;
		ipencap->sport = nla_get_be16(data[IFLA_GRE_ENCAP_SPORT]);
	}

	if (data[IFLA_GRE_ENCAP_DPORT]) {
		ret = true;
		ipencap->dport = nla_get_be16(data[IFLA_GRE_ENCAP_DPORT]);
	}

	return ret;
}