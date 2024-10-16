static int vbg_pci_probe(struct pci_dev *pci, const struct pci_device_id *id)
{
	struct device *dev = &pci->dev;
	resource_size_t io, io_len, mmio, mmio_len;
	struct vmmdev_memory *vmmdev;
	struct vbg_dev *gdev;
	int ret;

	gdev = devm_kzalloc(dev, sizeof(*gdev), GFP_KERNEL);
	if (!gdev)
		return -ENOMEM;

	ret = pci_enable_device(pci);
	if (ret != 0) {
		vbg_err("vboxguest: Error enabling device: %d\n", ret);
		return ret;
	}

	ret = -ENODEV;

	io = pci_resource_start(pci, 0);
	io_len = pci_resource_len(pci, 0);
	if (!io || !io_len) {
		vbg_err("vboxguest: Error IO-port resource (0) is missing\n");
		goto err_disable_pcidev;
	}
	if (devm_request_region(dev, io, io_len, DEVICE_NAME) == NULL) {
		vbg_err("vboxguest: Error could not claim IO resource\n");
		ret = -EBUSY;
		goto err_disable_pcidev;
	}

	mmio = pci_resource_start(pci, 1);
	mmio_len = pci_resource_len(pci, 1);
	if (!mmio || !mmio_len) {
		vbg_err("vboxguest: Error MMIO resource (1) is missing\n");
		goto err_disable_pcidev;
	}

	if (devm_request_mem_region(dev, mmio, mmio_len, DEVICE_NAME) == NULL) {
		vbg_err("vboxguest: Error could not claim MMIO resource\n");
		ret = -EBUSY;
		goto err_disable_pcidev;
	}

	vmmdev = devm_ioremap(dev, mmio, mmio_len);
	if (!vmmdev) {
		vbg_err("vboxguest: Error ioremap failed; MMIO addr=%pap size=%pap\n",
			&mmio, &mmio_len);
		goto err_disable_pcidev;
	}

	/* Validate MMIO region version and size. */
	if (vmmdev->version != VMMDEV_MEMORY_VERSION ||
	    vmmdev->size < 32 || vmmdev->size > mmio_len) {
		vbg_err("vboxguest: Bogus VMMDev memory; version=%08x (expected %08x) size=%d (expected <= %d)\n",
			vmmdev->version, VMMDEV_MEMORY_VERSION,
			vmmdev->size, (int)mmio_len);
		goto err_disable_pcidev;
	}

	gdev->io_port = io;
	gdev->mmio = vmmdev;
	gdev->dev = dev;
	gdev->misc_device.minor = MISC_DYNAMIC_MINOR;
	gdev->misc_device.name = DEVICE_NAME;
	gdev->misc_device.fops = &vbg_misc_device_fops;
	gdev->misc_device_user.minor = MISC_DYNAMIC_MINOR;
	gdev->misc_device_user.name = DEVICE_NAME_USER;
	gdev->misc_device_user.fops = &vbg_misc_device_user_fops;

	ret = vbg_core_init(gdev, VMMDEV_EVENT_MOUSE_POSITION_CHANGED);
	if (ret)
		goto err_disable_pcidev;

	ret = vbg_create_input_device(gdev);
	if (ret) {
		vbg_err("vboxguest: Error creating input device: %d\n", ret);
		goto err_vbg_core_exit;
	}

	ret = devm_request_irq(dev, pci->irq, vbg_core_isr, IRQF_SHARED,
			       DEVICE_NAME, gdev);
	if (ret) {
		vbg_err("vboxguest: Error requesting irq: %d\n", ret);
		goto err_vbg_core_exit;
	}

	ret = misc_register(&gdev->misc_device);
	if (ret) {
		vbg_err("vboxguest: Error misc_register %s failed: %d\n",
			DEVICE_NAME, ret);
		goto err_vbg_core_exit;
	}

	ret = misc_register(&gdev->misc_device_user);
	if (ret) {
		vbg_err("vboxguest: Error misc_register %s failed: %d\n",
			DEVICE_NAME_USER, ret);
		goto err_unregister_misc_device;
	}

	mutex_lock(&vbg_gdev_mutex);
	if (!vbg_gdev)
		vbg_gdev = gdev;
	else
		ret = -EBUSY;
	mutex_unlock(&vbg_gdev_mutex);

	if (ret) {
		vbg_err("vboxguest: Error more then 1 vbox guest pci device\n");
		goto err_unregister_misc_device_user;
	}

	pci_set_drvdata(pci, gdev);
	device_create_file(dev, &dev_attr_host_version);
	device_create_file(dev, &dev_attr_host_features);

	vbg_info("vboxguest: misc device minor %d, IRQ %d, I/O port %x, MMIO at %pap (size %pap)\n",
		 gdev->misc_device.minor, pci->irq, gdev->io_port,
		 &mmio, &mmio_len);

	return 0;

err_unregister_misc_device_user:
	misc_deregister(&gdev->misc_device_user);
err_unregister_misc_device:
	misc_deregister(&gdev->misc_device);
err_vbg_core_exit:
	vbg_core_exit(gdev);
err_disable_pcidev:
	pci_disable_device(pci);

	return ret;
}