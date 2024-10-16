static void ip6gre_tunnel_link(struct ip6gre_net *ign, struct ip6_tnl *t)
{
	struct ip6_tnl __rcu **tp = ip6gre_bucket(ign, t);

	rcu_assign_pointer(t->next, rtnl_dereference(*tp));
	rcu_assign_pointer(*tp, t);
}