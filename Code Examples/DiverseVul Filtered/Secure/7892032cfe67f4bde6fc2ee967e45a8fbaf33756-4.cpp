static inline struct ip6_tnl __rcu **ip6gre_bucket(struct ip6gre_net *ign,
		const struct ip6_tnl *t)
{
	return __ip6gre_bucket(ign, &t->parms);
}