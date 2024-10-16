static int ip6gre_tnl_change(struct ip6_tnl *t,
	const struct __ip6_tnl_parm *p, int set_mtu)
{
	t->parms.laddr = p->laddr;
	t->parms.raddr = p->raddr;
	t->parms.flags = p->flags;
	t->parms.hop_limit = p->hop_limit;
	t->parms.encap_limit = p->encap_limit;
	t->parms.flowinfo = p->flowinfo;
	t->parms.link = p->link;
	t->parms.proto = p->proto;
	t->parms.i_key = p->i_key;
	t->parms.o_key = p->o_key;
	t->parms.i_flags = p->i_flags;
	t->parms.o_flags = p->o_flags;
	dst_cache_reset(&t->dst_cache);
	ip6gre_tnl_link_config(t, set_mtu);
	return 0;
}