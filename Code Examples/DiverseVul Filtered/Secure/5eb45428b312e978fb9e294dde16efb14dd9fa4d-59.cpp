static int instantiate_veth(char *veth1, char *veth2)
{
	int ret;

	ret = lxc_veth_create(veth1, veth2);
	if (ret < 0) {
		usernic_error("Failed to create %s-%s : %s.\n", veth1, veth2,
			      strerror(-ret));
		return -1;
	}

	/* Changing the high byte of the mac address to 0xfe, the bridge
	 * interface will always keep the host's mac address and not take the
	 * mac address of a container. */
	ret = setup_private_host_hw_addr(veth1);
	if (ret < 0)
		usernic_error("Failed to change mac address of host interface "
			      "%s : %s\n", veth1, strerror(-ret));

	return netdev_set_flag(veth1, IFF_UP);
}