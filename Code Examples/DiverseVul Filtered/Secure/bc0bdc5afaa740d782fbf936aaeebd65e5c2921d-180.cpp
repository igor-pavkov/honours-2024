static struct cma_pernet *cma_pernet(struct net *net)
{
	return net_generic(net, cma_pernet_id);
}