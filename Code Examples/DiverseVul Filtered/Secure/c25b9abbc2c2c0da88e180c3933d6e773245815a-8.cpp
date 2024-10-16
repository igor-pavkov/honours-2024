static int skfp_init_one(struct pci_dev *pdev,
				const struct pci_device_id *ent)
{
	struct net_device *dev;
	struct s_smc *smc;	/* board pointer */
	void __iomem *mem;
	int err;

	PRINTK(KERN_INFO "entering skfp_init_one\n");

	if (num_boards == 0) 
		printk("%s\n", boot_msg);

	err = pci_enable_device(pdev);
	if (err)
		return err;

	err = pci_request_regions(pdev, "skfddi");
	if (err)
		goto err_out1;

	pci_set_master(pdev);

#ifdef MEM_MAPPED_IO
	if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
		printk(KERN_ERR "skfp: region is not an MMIO resource\n");
		err = -EIO;
		goto err_out2;
	}

	mem = ioremap(pci_resource_start(pdev, 0), 0x4000);
#else
	if (!(pci_resource_flags(pdev, 1) & IO_RESOURCE_IO)) {
		printk(KERN_ERR "skfp: region is not PIO resource\n");
		err = -EIO;
		goto err_out2;
	}

	mem = ioport_map(pci_resource_start(pdev, 1), FP_IO_LEN);
#endif
	if (!mem) {
		printk(KERN_ERR "skfp:  Unable to map register, "
				"FDDI adapter will be disabled.\n");
		err = -EIO;
		goto err_out2;
	}

	dev = alloc_fddidev(sizeof(struct s_smc));
	if (!dev) {
		printk(KERN_ERR "skfp: Unable to allocate fddi device, "
				"FDDI adapter will be disabled.\n");
		err = -ENOMEM;
		goto err_out3;
	}

	dev->irq = pdev->irq;
	dev->netdev_ops = &skfp_netdev_ops;

	SET_NETDEV_DEV(dev, &pdev->dev);

	/* Initialize board structure with bus-specific info */
	smc = netdev_priv(dev);
	smc->os.dev = dev;
	smc->os.bus_type = SK_BUS_TYPE_PCI;
	smc->os.pdev = *pdev;
	smc->os.QueueSkb = MAX_TX_QUEUE_LEN;
	smc->os.MaxFrameSize = MAX_FRAME_SIZE;
	smc->os.dev = dev;
	smc->hw.slot = -1;
	smc->hw.iop = mem;
	smc->os.ResetRequested = FALSE;
	skb_queue_head_init(&smc->os.SendSkbQueue);

	dev->base_addr = (unsigned long)mem;

	err = skfp_driver_init(dev);
	if (err)
		goto err_out4;

	err = register_netdev(dev);
	if (err)
		goto err_out5;

	++num_boards;
	pci_set_drvdata(pdev, dev);

	if ((pdev->subsystem_device & 0xff00) == 0x5500 ||
	    (pdev->subsystem_device & 0xff00) == 0x5800) 
		printk("%s: SysKonnect FDDI PCI adapter"
		       " found (SK-%04X)\n", dev->name,	
		       pdev->subsystem_device);
	else
		printk("%s: FDDI PCI adapter found\n", dev->name);

	return 0;
err_out5:
	if (smc->os.SharedMemAddr) 
		pci_free_consistent(pdev, smc->os.SharedMemSize,
				    smc->os.SharedMemAddr, 
				    smc->os.SharedMemDMA);
	pci_free_consistent(pdev, MAX_FRAME_SIZE,
			    smc->os.LocalRxBuffer, smc->os.LocalRxBufferDMA);
err_out4:
	free_netdev(dev);
err_out3:
#ifdef MEM_MAPPED_IO
	iounmap(mem);
#else
	ioport_unmap(mem);
#endif
err_out2:
	pci_release_regions(pdev);
err_out1:
	pci_disable_device(pdev);
	return err;
}