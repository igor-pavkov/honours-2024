static int __net_init xfrm_statistics_init(struct net *net)
{
	int rv;
	net->mib.xfrm_statistics = alloc_percpu(struct linux_xfrm_mib);
	if (!net->mib.xfrm_statistics)
		return -ENOMEM;
	rv = xfrm_proc_init(net);
	if (rv < 0)
		free_percpu(net->mib.xfrm_statistics);
	return rv;
}