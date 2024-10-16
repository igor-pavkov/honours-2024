static void cma_exit_net(struct net *net)
{
	struct cma_pernet *pernet = cma_pernet(net);

	WARN_ON(!xa_empty(&pernet->tcp_ps));
	WARN_ON(!xa_empty(&pernet->udp_ps));
	WARN_ON(!xa_empty(&pernet->ipoib_ps));
	WARN_ON(!xa_empty(&pernet->ib_ps));
}