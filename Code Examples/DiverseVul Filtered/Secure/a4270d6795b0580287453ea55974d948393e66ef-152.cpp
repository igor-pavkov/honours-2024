void net_dec_ingress_queue(void)
{
	static_branch_dec(&ingress_needed_key);
}