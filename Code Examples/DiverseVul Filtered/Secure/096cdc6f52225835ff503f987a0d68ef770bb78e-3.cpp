static int ec_device_probe(struct platform_device *pdev)
{
	int retval = -ENOMEM;
	struct device *dev = &pdev->dev;
	struct cros_ec_platform *ec_platform = dev_get_platdata(dev);
	dev_t devno = MKDEV(ec_major, pdev->id);
	struct cros_ec_dev *ec = kzalloc(sizeof(*ec), GFP_KERNEL);

	if (!ec)
		return retval;

	dev_set_drvdata(dev, ec);
	ec->ec_dev = dev_get_drvdata(dev->parent);
	ec->dev = dev;
	ec->cmd_offset = ec_platform->cmd_offset;
	device_initialize(&ec->class_dev);
	cdev_init(&ec->cdev, &fops);

	/*
	 * Add the character device
	 * Link cdev to the class device to be sure device is not used
	 * before unbinding it.
	 */
	ec->cdev.kobj.parent = &ec->class_dev.kobj;
	retval = cdev_add(&ec->cdev, devno, 1);
	if (retval) {
		dev_err(dev, ": failed to add character device\n");
		goto cdev_add_failed;
	}

	/*
	 * Add the class device
	 * Link to the character device for creating the /dev entry
	 * in devtmpfs.
	 */
	ec->class_dev.devt = ec->cdev.dev;
	ec->class_dev.class = &cros_class;
	ec->class_dev.parent = dev;
	ec->class_dev.release = __remove;

	retval = dev_set_name(&ec->class_dev, "%s", ec_platform->ec_name);
	if (retval) {
		dev_err(dev, "dev_set_name failed => %d\n", retval);
		goto set_named_failed;
	}

	retval = device_add(&ec->class_dev);
	if (retval) {
		dev_err(dev, "device_register failed => %d\n", retval);
		goto dev_reg_failed;
	}

	return 0;

dev_reg_failed:
set_named_failed:
	dev_set_drvdata(dev, NULL);
	cdev_del(&ec->cdev);
cdev_add_failed:
	kfree(ec);
	return retval;
}