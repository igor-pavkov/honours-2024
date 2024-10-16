void net_dec_egress_queue(void)
{
	static_branch_dec(&egress_needed_key);
}