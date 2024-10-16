static void skfp_ctl_set_multicast_list_wo_lock(struct net_device *dev)
{
	struct s_smc *smc = netdev_priv(dev);
	struct dev_mc_list *dmi;	/* ptr to multicast addr entry */
	int i;

	/* Enable promiscuous mode, if necessary */
	if (dev->flags & IFF_PROMISC) {
		mac_drv_rx_mode(smc, RX_ENABLE_PROMISC);
		PRINTK(KERN_INFO "PROMISCUOUS MODE ENABLED\n");
	}
	/* Else, update multicast address table */
	else {
		mac_drv_rx_mode(smc, RX_DISABLE_PROMISC);
		PRINTK(KERN_INFO "PROMISCUOUS MODE DISABLED\n");

		// Reset all MC addresses
		mac_clear_multicast(smc);
		mac_drv_rx_mode(smc, RX_DISABLE_ALLMULTI);

		if (dev->flags & IFF_ALLMULTI) {
			mac_drv_rx_mode(smc, RX_ENABLE_ALLMULTI);
			PRINTK(KERN_INFO "ENABLE ALL MC ADDRESSES\n");
		} else if (dev->mc_count > 0) {
			if (dev->mc_count <= FPMAX_MULTICAST) {
				/* use exact filtering */

				// point to first multicast addr
				dmi = dev->mc_list;

				for (i = 0; i < dev->mc_count; i++) {
					mac_add_multicast(smc, 
							  (struct fddi_addr *)dmi->dmi_addr, 
							  1);

					PRINTK(KERN_INFO "ENABLE MC ADDRESS:");
					PRINTK(" %02x %02x %02x ",
					       dmi->dmi_addr[0],
					       dmi->dmi_addr[1],
					       dmi->dmi_addr[2]);
					PRINTK("%02x %02x %02x\n",
					       dmi->dmi_addr[3],
					       dmi->dmi_addr[4],
					       dmi->dmi_addr[5]);
					dmi = dmi->next;
				}	// for

			} else {	// more MC addresses than HW supports

				mac_drv_rx_mode(smc, RX_ENABLE_ALLMULTI);
				PRINTK(KERN_INFO "ENABLE ALL MC ADDRESSES\n");
			}
		} else {	// no MC addresses

			PRINTK(KERN_INFO "DISABLE ALL MC ADDRESSES\n");
		}

		/* Update adapter filters */
		mac_update_multicast(smc);
	}
	return;
}				// skfp_ctl_set_multicast_list_wo_lock