static int cma_ps_alloc(struct net *net, enum rdma_ucm_port_space ps,
			struct rdma_bind_list *bind_list, int snum)
{
	struct xarray *xa = cma_pernet_xa(net, ps);

	return xa_insert(xa, snum, bind_list, GFP_KERNEL);
}