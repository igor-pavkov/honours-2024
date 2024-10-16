static void __devexit skfp_remove_one(struct pci_dev *pdev)
{
	struct net_device *p = pci_get_drvdata(pdev);
	struct s_smc *lp = netdev_priv(p);

	unregister_netdev(p);

	if (lp->os.SharedMemAddr) {
		pci_free_consistent(&lp->os.pdev,
				    lp->os.SharedMemSize,
				    lp->os.SharedMemAddr,
				    lp->os.SharedMemDMA);
		lp->os.SharedMemAddr = NULL;
	}
	if (lp->os.LocalRxBuffer) {
		pci_free_consistent(&lp->os.pdev,
				    MAX_FRAME_SIZE,
				    lp->os.LocalRxBuffer,
				    lp->os.LocalRxBufferDMA);
		lp->os.LocalRxBuffer = NULL;
	}
#ifdef MEM_MAPPED_IO
	iounmap(lp->hw.iop);
#else
	ioport_unmap(lp->hw.iop);
#endif
	pci_release_regions(pdev);
	free_netdev(p);

	pci_disable_device(pdev);
	pci_set_drvdata(pdev, NULL);
}