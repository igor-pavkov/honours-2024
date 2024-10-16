static void fm10k_reset_msix_capability(struct fm10k_intfc *interface)
{
	pci_disable_msix(interface->pdev);
	kfree(interface->msix_entries);
	interface->msix_entries = NULL;
}