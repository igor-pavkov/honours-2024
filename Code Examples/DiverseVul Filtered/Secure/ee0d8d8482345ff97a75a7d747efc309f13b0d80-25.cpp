void ipxitf_down(struct ipx_interface *intrfc)
{
	spin_lock_bh(&ipx_interfaces_lock);
	__ipxitf_down(intrfc);
	spin_unlock_bh(&ipx_interfaces_lock);
}