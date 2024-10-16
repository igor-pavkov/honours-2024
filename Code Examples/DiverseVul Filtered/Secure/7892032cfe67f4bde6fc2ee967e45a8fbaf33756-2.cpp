static void ip6gre_tnl_parm_to_user(struct ip6_tnl_parm2 *u,
	const struct __ip6_tnl_parm *p)
{
	u->proto = IPPROTO_GRE;
	u->laddr = p->laddr;
	u->raddr = p->raddr;
	u->flags = p->flags;
	u->hop_limit = p->hop_limit;
	u->encap_limit = p->encap_limit;
	u->flowinfo = p->flowinfo;
	u->link = p->link;
	u->i_key = p->i_key;
	u->o_key = p->o_key;
	u->i_flags = gre_tnl_flags_to_gre_flags(p->i_flags);
	u->o_flags = gre_tnl_flags_to_gre_flags(p->o_flags);
	memcpy(u->name, p->name, sizeof(u->name));
}