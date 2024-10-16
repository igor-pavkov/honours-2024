static __exit void ipxitf_cleanup(void)
{
	struct ipx_interface *i, *tmp;

	spin_lock_bh(&ipx_interfaces_lock);
	list_for_each_entry_safe(i, tmp, &ipx_interfaces, node)
		__ipxitf_put(i);
	spin_unlock_bh(&ipx_interfaces_lock);
}