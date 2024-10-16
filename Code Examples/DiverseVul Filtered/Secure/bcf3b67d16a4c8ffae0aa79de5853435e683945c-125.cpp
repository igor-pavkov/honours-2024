static void megasas_shutdown(struct pci_dev *pdev)
{
	struct megasas_instance *instance = pci_get_drvdata(pdev);

	instance->unload = 1;

	if (megasas_wait_for_adapter_operational(instance))
		goto skip_firing_dcmds;

	megasas_flush_cache(instance);
	megasas_shutdown_controller(instance, MR_DCMD_CTRL_SHUTDOWN);

skip_firing_dcmds:
	instance->instancet->disable_intr(instance);
	megasas_destroy_irqs(instance);

	if (instance->msix_vectors)
		pci_free_irq_vectors(instance->pdev);
}