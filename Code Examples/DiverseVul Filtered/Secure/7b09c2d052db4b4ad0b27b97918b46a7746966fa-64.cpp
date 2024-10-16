static void node_free(struct net *net, struct fib6_node *fn)
{
	call_rcu(&fn->rcu, node_free_rcu);
	net->ipv6.rt6_stats->fib_nodes--;
}