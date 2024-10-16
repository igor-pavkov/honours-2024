void sock_prot_inuse_add(struct net *net, struct proto *prot, int val)
{
	int cpu = smp_processor_id();
	per_cpu_ptr(net->core.inuse, cpu)->val[prot->inuse_idx] += val;
}