static struct rdma_bind_list *cma_ps_find(struct net *net,
					  enum rdma_ucm_port_space ps, int snum)
{
	struct xarray *xa = cma_pernet_xa(net, ps);

	return xa_load(xa, snum);
}