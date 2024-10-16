static void cma_ps_remove(struct net *net, enum rdma_ucm_port_space ps,
			  int snum)
{
	struct xarray *xa = cma_pernet_xa(net, ps);

	xa_erase(xa, snum);
}