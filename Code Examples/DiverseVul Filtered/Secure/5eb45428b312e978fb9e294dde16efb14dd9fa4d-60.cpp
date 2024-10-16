static int create_nic(char *nic, char *br, int pid, char **cnic)
{
	char veth1buf[IFNAMSIZ], veth2buf[IFNAMSIZ];
	int mtu, ret;

	ret = snprintf(veth1buf, IFNAMSIZ, "%s", nic);
	if (ret < 0 || ret >= IFNAMSIZ) {
		usernic_error("%s", "Could not create nic name\n");
		return -1;
	}

	ret = snprintf(veth2buf, IFNAMSIZ, "%sp", veth1buf);
	if (ret < 0 || ret >= IFNAMSIZ) {
		usernic_error("%s\n", "Could not create nic name");
		return -1;
	}
	/* create the nics */
	ret = instantiate_veth(veth1buf, veth2buf);
	if (ret < 0) {
		usernic_error("%s", "Error creating veth tunnel\n");
		return -1;
	}

	if (strcmp(br, "none")) {
		/* copy the bridge's mtu to both ends */
		mtu = get_mtu(br);
		if (mtu > 0) {
			ret = lxc_netdev_set_mtu(veth1buf, mtu);
			if (ret < 0) {
				usernic_error("Failed to set mtu to %d on %s\n",
					      mtu, veth1buf);
				goto out_del;
			}

			ret = lxc_netdev_set_mtu(veth2buf, mtu);
			if (ret < 0) {
				usernic_error("Failed to set mtu to %d on %s\n",
					      mtu, veth2buf);
				goto out_del;
			}
		}

		/* attach veth1 to bridge */
		ret = lxc_bridge_attach(br, veth1buf);
		if (ret < 0) {
			usernic_error("Error attaching %s to %s\n", veth1buf, br);
			goto out_del;
		}
	}

	/* pass veth2 to target netns */
	ret = lxc_netdev_move_by_name(veth2buf, pid, NULL);
	if (ret < 0) {
		usernic_error("Error moving %s to network namespace of %d\n",
			      veth2buf, pid);
		goto out_del;
	}

	*cnic = strdup(veth2buf);
	if (!*cnic) {
		usernic_error("Failed to copy string \"%s\"\n", veth2buf);
		return -1;
	}

	return 0;

out_del:
	lxc_netdev_delete_by_name(veth1buf);
	return -1;
}