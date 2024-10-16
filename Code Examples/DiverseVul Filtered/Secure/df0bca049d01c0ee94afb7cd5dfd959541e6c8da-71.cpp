void sock_prot_inuse_add(struct net *net, struct proto *prot, int val)
{
	__get_cpu_var(prot_inuse).val[prot->inuse_idx] += val;
}