struct xarray *cma_pernet_xa(struct net *net, enum rdma_ucm_port_space ps)
{
	struct cma_pernet *pernet = cma_pernet(net);

	switch (ps) {
	case RDMA_PS_TCP:
		return &pernet->tcp_ps;
	case RDMA_PS_UDP:
		return &pernet->udp_ps;
	case RDMA_PS_IPOIB:
		return &pernet->ipoib_ps;
	case RDMA_PS_IB:
		return &pernet->ib_ps;
	default:
		return NULL;
	}
}