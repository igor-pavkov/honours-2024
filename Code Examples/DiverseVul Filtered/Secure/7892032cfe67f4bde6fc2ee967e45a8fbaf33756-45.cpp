static void ip6gre_netlink_parms(struct nlattr *data[],
				struct __ip6_tnl_parm *parms)
{
	memset(parms, 0, sizeof(*parms));

	if (!data)
		return;

	if (data[IFLA_GRE_LINK])
		parms->link = nla_get_u32(data[IFLA_GRE_LINK]);

	if (data[IFLA_GRE_IFLAGS])
		parms->i_flags = gre_flags_to_tnl_flags(
				nla_get_be16(data[IFLA_GRE_IFLAGS]));

	if (data[IFLA_GRE_OFLAGS])
		parms->o_flags = gre_flags_to_tnl_flags(
				nla_get_be16(data[IFLA_GRE_OFLAGS]));

	if (data[IFLA_GRE_IKEY])
		parms->i_key = nla_get_be32(data[IFLA_GRE_IKEY]);

	if (data[IFLA_GRE_OKEY])
		parms->o_key = nla_get_be32(data[IFLA_GRE_OKEY]);

	if (data[IFLA_GRE_LOCAL])
		parms->laddr = nla_get_in6_addr(data[IFLA_GRE_LOCAL]);

	if (data[IFLA_GRE_REMOTE])
		parms->raddr = nla_get_in6_addr(data[IFLA_GRE_REMOTE]);

	if (data[IFLA_GRE_TTL])
		parms->hop_limit = nla_get_u8(data[IFLA_GRE_TTL]);

	if (data[IFLA_GRE_ENCAP_LIMIT])
		parms->encap_limit = nla_get_u8(data[IFLA_GRE_ENCAP_LIMIT]);

	if (data[IFLA_GRE_FLOWINFO])
		parms->flowinfo = nla_get_be32(data[IFLA_GRE_FLOWINFO]);

	if (data[IFLA_GRE_FLAGS])
		parms->flags = nla_get_u32(data[IFLA_GRE_FLAGS]);
}