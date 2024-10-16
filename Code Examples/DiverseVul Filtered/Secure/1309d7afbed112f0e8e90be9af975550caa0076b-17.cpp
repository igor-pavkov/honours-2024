void tpm_remove_hardware(struct device *dev)
{
	struct tpm_chip *chip = dev_get_drvdata(dev);

	if (chip == NULL) {
		dev_err(dev, "No device data found\n");
		return;
	}

	spin_lock(&driver_lock);
	list_del_rcu(&chip->list);
	spin_unlock(&driver_lock);
	synchronize_rcu();

	misc_deregister(&chip->vendor.miscdev);
	sysfs_remove_group(&dev->kobj, chip->vendor.attr_group);
	tpm_bios_log_teardown(chip->bios_dir);

	/* write it this way to be explicit (chip->dev == dev) */
	put_device(chip->dev);
}