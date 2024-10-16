static int cma_init_net(struct net *net)
{
	struct cma_pernet *pernet = cma_pernet(net);

	xa_init(&pernet->tcp_ps);
	xa_init(&pernet->udp_ps);
	xa_init(&pernet->ipoib_ps);
	xa_init(&pernet->ib_ps);

	return 0;
}