int ip_frag_mem(struct net *net)
{
	return atomic_read(&net->ipv4.frags.mem);
}