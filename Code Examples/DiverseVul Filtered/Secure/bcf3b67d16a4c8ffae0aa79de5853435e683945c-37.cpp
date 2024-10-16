static int __init megasas_init(void)
{
	int rval;

	/*
	 * Booted in kdump kernel, minimize memory footprints by
	 * disabling few features
	 */
	if (reset_devices) {
		msix_vectors = 1;
		rdpq_enable = 0;
		dual_qdepth_disable = 1;
	}

	/*
	 * Announce driver version and other information
	 */
	pr_info("megasas: %s\n", MEGASAS_VERSION);

	spin_lock_init(&poll_aen_lock);

	support_poll_for_event = 2;
	support_device_change = 1;
	support_nvme_encapsulation = true;

	memset(&megasas_mgmt_info, 0, sizeof(megasas_mgmt_info));

	/*
	 * Register character device node
	 */
	rval = register_chrdev(0, "megaraid_sas_ioctl", &megasas_mgmt_fops);

	if (rval < 0) {
		printk(KERN_DEBUG "megasas: failed to open device node\n");
		return rval;
	}

	megasas_mgmt_majorno = rval;

	/*
	 * Register ourselves as PCI hotplug module
	 */
	rval = pci_register_driver(&megasas_pci_driver);

	if (rval) {
		printk(KERN_DEBUG "megasas: PCI hotplug registration failed \n");
		goto err_pcidrv;
	}

	rval = driver_create_file(&megasas_pci_driver.driver,
				  &driver_attr_version);
	if (rval)
		goto err_dcf_attr_ver;

	rval = driver_create_file(&megasas_pci_driver.driver,
				  &driver_attr_release_date);
	if (rval)
		goto err_dcf_rel_date;

	rval = driver_create_file(&megasas_pci_driver.driver,
				&driver_attr_support_poll_for_event);
	if (rval)
		goto err_dcf_support_poll_for_event;

	rval = driver_create_file(&megasas_pci_driver.driver,
				  &driver_attr_dbg_lvl);
	if (rval)
		goto err_dcf_dbg_lvl;
	rval = driver_create_file(&megasas_pci_driver.driver,
				&driver_attr_support_device_change);
	if (rval)
		goto err_dcf_support_device_change;

	rval = driver_create_file(&megasas_pci_driver.driver,
				  &driver_attr_support_nvme_encapsulation);
	if (rval)
		goto err_dcf_support_nvme_encapsulation;

	return rval;

err_dcf_support_nvme_encapsulation:
	driver_remove_file(&megasas_pci_driver.driver,
			   &driver_attr_support_device_change);

err_dcf_support_device_change:
	driver_remove_file(&megasas_pci_driver.driver,
			   &driver_attr_dbg_lvl);
err_dcf_dbg_lvl:
	driver_remove_file(&megasas_pci_driver.driver,
			&driver_attr_support_poll_for_event);
err_dcf_support_poll_for_event:
	driver_remove_file(&megasas_pci_driver.driver,
			   &driver_attr_release_date);
err_dcf_rel_date:
	driver_remove_file(&megasas_pci_driver.driver, &driver_attr_version);
err_dcf_attr_ver:
	pci_unregister_driver(&megasas_pci_driver);
err_pcidrv:
	unregister_chrdev(megasas_mgmt_majorno, "megaraid_sas_ioctl");
	return rval;
}