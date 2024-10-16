static void ip_evictor(struct net *net)
{
	int evicted;

	evicted = inet_frag_evictor(&net->ipv4.frags, &ip4_frags);
	if (evicted)
		IP_ADD_STATS_BH(net, IPSTATS_MIB_REASMFAILS, evicted);
}