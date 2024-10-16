static struct ip6_tnl __rcu **__ip6gre_bucket(struct ip6gre_net *ign,
		const struct __ip6_tnl_parm *p)
{
	const struct in6_addr *remote = &p->raddr;
	const struct in6_addr *local = &p->laddr;
	unsigned int h = HASH_KEY(p->i_key);
	int prio = 0;

	if (!ipv6_addr_any(local))
		prio |= 1;
	if (!ipv6_addr_any(remote) && !ipv6_addr_is_multicast(remote)) {
		prio |= 2;
		h ^= HASH_ADDR(remote);
	}

	return &ign->tunnels[prio][h];
}