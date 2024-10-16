static void vbg_pci_remove(struct pci_dev *pci)
{
	struct vbg_dev *gdev = pci_get_drvdata(pci);

	mutex_lock(&vbg_gdev_mutex);
	vbg_gdev = NULL;
	mutex_unlock(&vbg_gdev_mutex);

	device_remove_file(gdev->dev, &dev_attr_host_features);
	device_remove_file(gdev->dev, &dev_attr_host_version);
	misc_deregister(&gdev->misc_device_user);
	misc_deregister(&gdev->misc_device);
	vbg_core_exit(gdev);
	pci_disable_device(pci);
}