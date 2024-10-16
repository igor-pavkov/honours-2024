static struct ipx_interface *__ipxitf_find_using_phys(struct net_device *dev,
						      __be16 datalink)
{
	struct ipx_interface *i;

	list_for_each_entry(i, &ipx_interfaces, node)
		if (i->if_dev == dev && i->if_dlink_type == datalink)
			goto out;
	i = NULL;
out:
	return i;
}