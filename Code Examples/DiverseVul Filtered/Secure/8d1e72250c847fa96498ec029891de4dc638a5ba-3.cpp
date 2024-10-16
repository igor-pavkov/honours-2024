void unregister_control_device_interface(struct bcm_mini_adapter *Adapter)
{
	if (Adapter->major > 0) {
		device_destroy(bcm_class, MKDEV(Adapter->major, 0));
		unregister_chrdev(Adapter->major, DEV_NAME);
	}
}