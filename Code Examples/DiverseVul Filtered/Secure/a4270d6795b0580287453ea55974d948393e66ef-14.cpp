void net_inc_ingress_queue(void)
{
	static_branch_inc(&ingress_needed_key);
}