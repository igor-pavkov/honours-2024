static  int skfp_driver_init(struct net_device *dev)
{
	struct s_smc *smc = netdev_priv(dev);
	skfddi_priv *bp = &smc->os;
	int err = -EIO;

	PRINTK(KERN_INFO "entering skfp_driver_init\n");

	// set the io address in private structures
	bp->base_addr = dev->base_addr;

	// Get the interrupt level from the PCI Configuration Table
	smc->hw.irq = dev->irq;

	spin_lock_init(&bp->DriverLock);
	
	// Allocate invalid frame
	bp->LocalRxBuffer = pci_alloc_consistent(&bp->pdev, MAX_FRAME_SIZE, &bp->LocalRxBufferDMA);
	if (!bp->LocalRxBuffer) {
		printk("could not allocate mem for ");
		printk("LocalRxBuffer: %d byte\n", MAX_FRAME_SIZE);
		goto fail;
	}

	// Determine the required size of the 'shared' memory area.
	bp->SharedMemSize = mac_drv_check_space();
	PRINTK(KERN_INFO "Memory for HWM: %ld\n", bp->SharedMemSize);
	if (bp->SharedMemSize > 0) {
		bp->SharedMemSize += 16;	// for descriptor alignment

		bp->SharedMemAddr = pci_alloc_consistent(&bp->pdev,
							 bp->SharedMemSize,
							 &bp->SharedMemDMA);
		if (!bp->SharedMemSize) {
			printk("could not allocate mem for ");
			printk("hardware module: %ld byte\n",
			       bp->SharedMemSize);
			goto fail;
		}
		bp->SharedMemHeap = 0;	// Nothing used yet.

	} else {
		bp->SharedMemAddr = NULL;
		bp->SharedMemHeap = 0;
	}			// SharedMemSize > 0

	memset(bp->SharedMemAddr, 0, bp->SharedMemSize);

	card_stop(smc);		// Reset adapter.

	PRINTK(KERN_INFO "mac_drv_init()..\n");
	if (mac_drv_init(smc) != 0) {
		PRINTK(KERN_INFO "mac_drv_init() failed.\n");
		goto fail;
	}
	read_address(smc, NULL);
	PRINTK(KERN_INFO "HW-Addr: %02x %02x %02x %02x %02x %02x\n",
	       smc->hw.fddi_canon_addr.a[0],
	       smc->hw.fddi_canon_addr.a[1],
	       smc->hw.fddi_canon_addr.a[2],
	       smc->hw.fddi_canon_addr.a[3],
	       smc->hw.fddi_canon_addr.a[4],
	       smc->hw.fddi_canon_addr.a[5]);
	memcpy(dev->dev_addr, smc->hw.fddi_canon_addr.a, 6);

	smt_reset_defaults(smc, 0);

	return (0);

fail:
	if (bp->SharedMemAddr) {
		pci_free_consistent(&bp->pdev,
				    bp->SharedMemSize,
				    bp->SharedMemAddr,
				    bp->SharedMemDMA);
		bp->SharedMemAddr = NULL;
	}
	if (bp->LocalRxBuffer) {
		pci_free_consistent(&bp->pdev, MAX_FRAME_SIZE,
				    bp->LocalRxBuffer, bp->LocalRxBufferDMA);
		bp->LocalRxBuffer = NULL;
	}
	return err;
}				// skfp_driver_init