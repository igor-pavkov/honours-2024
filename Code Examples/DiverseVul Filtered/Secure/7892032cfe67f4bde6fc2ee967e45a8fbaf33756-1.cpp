static void ip6gre_tnl_parm_from_user(struct __ip6_tnl_parm *p,
	const struct ip6_tnl_parm2 *u)
{
	p->laddr = u->laddr;
	p->raddr = u->raddr;
	p->flags = u->flags;
	p->hop_limit = u->hop_limit;
	p->encap_limit = u->encap_limit;
	p->flowinfo = u->flowinfo;
	p->link = u->link;
	p->i_key = u->i_key;
	p->o_key = u->o_key;
	p->i_flags = gre_flags_to_tnl_flags(u->i_flags);
	p->o_flags = gre_flags_to_tnl_flags(u->o_flags);
	memcpy(p->name, u->name, sizeof(u->name));
}