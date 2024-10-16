void megasas_do_ocr(struct megasas_instance *instance)
{
	if ((instance->pdev->device == PCI_DEVICE_ID_LSI_SAS1064R) ||
	(instance->pdev->device == PCI_DEVICE_ID_DELL_PERC5) ||
	(instance->pdev->device == PCI_DEVICE_ID_LSI_VERDE_ZCR)) {
		*instance->consumer = cpu_to_le32(MEGASAS_ADPRESET_INPROG_SIGN);
	}
	instance->instancet->disable_intr(instance);
	atomic_set(&instance->adprecovery, MEGASAS_ADPRESET_SM_INFAULT);
	instance->issuepend_done = 0;

	atomic_set(&instance->fw_outstanding, 0);
	megasas_internal_reset_defer_cmds(instance);
	process_fw_state_change_wq(&instance->work_init);
}