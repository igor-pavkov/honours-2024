static int aac_get_pci_info(struct aac_dev* dev, void __user *arg)
{
	struct aac_pci_info pci_info;

	pci_info.bus = dev->pdev->bus->number;
	pci_info.slot = PCI_SLOT(dev->pdev->devfn);

	if (copy_to_user(arg, &pci_info, sizeof(struct aac_pci_info))) {
		dprintk((KERN_DEBUG "aacraid: Could not copy pci info\n"));
		return -EFAULT;
	}
	return 0;
}