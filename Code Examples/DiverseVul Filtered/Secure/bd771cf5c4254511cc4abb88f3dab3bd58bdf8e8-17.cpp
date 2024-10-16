static void smtcfb_pci_remove(struct pci_dev *pdev)
{
	struct smtcfb_info *sfb;

	sfb = pci_get_drvdata(pdev);
	smtc_unmap_smem(sfb);
	smtc_unmap_mmio(sfb);
	unregister_framebuffer(sfb->fb);
	framebuffer_release(sfb->fb);
	pci_release_region(pdev, 0);
	pci_disable_device(pdev);
}