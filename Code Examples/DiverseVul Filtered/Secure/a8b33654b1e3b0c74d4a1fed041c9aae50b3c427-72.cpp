static int pci_remap_base(struct pci_dev *pcidev, unsigned int offset, 
		unsigned int address, unsigned int size) 
{
#if 0
	struct resource *root;
	unsigned index = (offset - 0x10) >> 2;
#endif

	pci_write_config_dword(pcidev, offset, address);
#if 0
	root = pcidev->resource[index].parent;
	release_resource(&pcidev->resource[index]);
	address &= ~0x1;
	pcidev->resource[index].start = address;
	pcidev->resource[index].end	  = address + size - 1;

	if (request_resource(root, &pcidev->resource[index]) != NULL)
	{
		printk(KERN_ERR "pci remap conflict!! 0x%x\n", address);
		return (-1);
	}
#endif

	return (0);
}