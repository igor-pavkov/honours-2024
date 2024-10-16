void net_inc_egress_queue(void)
{
	static_branch_inc(&egress_needed_key);
}