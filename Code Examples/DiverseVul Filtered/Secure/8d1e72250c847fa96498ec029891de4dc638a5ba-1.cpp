int register_control_device_interface(struct bcm_mini_adapter *Adapter)
{

	if (Adapter->major > 0)
		return Adapter->major;

	Adapter->major = register_chrdev(0, DEV_NAME, &bcm_fops);
	if (Adapter->major < 0) {
		pr_err(DRV_NAME ": could not created character device\n");
		return Adapter->major;
	}

	Adapter->pstCreatedClassDevice = device_create(bcm_class, NULL,
						MKDEV(Adapter->major, 0),
						Adapter, DEV_NAME);

	if (IS_ERR(Adapter->pstCreatedClassDevice)) {
		pr_err(DRV_NAME ": class device create failed\n");
		unregister_chrdev(Adapter->major, DEV_NAME);
		return PTR_ERR(Adapter->pstCreatedClassDevice);
	}

	return 0;
}