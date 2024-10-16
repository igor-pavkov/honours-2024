struct tpm_chip *tpm_register_hardware(struct device *dev,
					const struct tpm_vendor_specific *entry)
{
#define DEVNAME_SIZE 7

	char *devname;
	struct tpm_chip *chip;

	/* Driver specific per-device data */
	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	devname = kmalloc(DEVNAME_SIZE, GFP_KERNEL);

	if (chip == NULL || devname == NULL)
		goto out_free;

	mutex_init(&chip->buffer_mutex);
	mutex_init(&chip->tpm_mutex);
	INIT_LIST_HEAD(&chip->list);

	INIT_WORK(&chip->work, timeout_work);

	setup_timer(&chip->user_read_timer, user_reader_timeout,
			(unsigned long)chip);

	memcpy(&chip->vendor, entry, sizeof(struct tpm_vendor_specific));

	chip->dev_num = find_first_zero_bit(dev_mask, TPM_NUM_DEVICES);

	if (chip->dev_num >= TPM_NUM_DEVICES) {
		dev_err(dev, "No available tpm device numbers\n");
		goto out_free;
	} else if (chip->dev_num == 0)
		chip->vendor.miscdev.minor = TPM_MINOR;
	else
		chip->vendor.miscdev.minor = MISC_DYNAMIC_MINOR;

	set_bit(chip->dev_num, dev_mask);

	scnprintf(devname, DEVNAME_SIZE, "%s%d", "tpm", chip->dev_num);
	chip->vendor.miscdev.name = devname;

	chip->vendor.miscdev.parent = dev;
	chip->dev = get_device(dev);
	chip->release = dev->release;
	dev->release = tpm_dev_release;
	dev_set_drvdata(dev, chip);

	if (misc_register(&chip->vendor.miscdev)) {
		dev_err(chip->dev,
			"unable to misc_register %s, minor %d\n",
			chip->vendor.miscdev.name,
			chip->vendor.miscdev.minor);
		put_device(chip->dev);
		return NULL;
	}

	if (sysfs_create_group(&dev->kobj, chip->vendor.attr_group)) {
		misc_deregister(&chip->vendor.miscdev);
		put_device(chip->dev);

		return NULL;
	}

	chip->bios_dir = tpm_bios_log_setup(devname);

	/* Make chip available */
	spin_lock(&driver_lock);
	list_add_rcu(&chip->list, &tpm_chip_list);
	spin_unlock(&driver_lock);

	return chip;

out_free:
	kfree(chip);
	kfree(devname);
	return NULL;
}