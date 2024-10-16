static void xfrm_statistics_fini(struct net *net)
{
	xfrm_proc_fini(net);
	free_percpu(net->mib.xfrm_statistics);
}