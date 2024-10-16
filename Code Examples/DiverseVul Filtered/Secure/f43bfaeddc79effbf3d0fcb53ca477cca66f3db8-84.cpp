static void atl2_shutdown(struct pci_dev *pdev)
{
	atl2_suspend(pdev, PMSG_SUSPEND);
}