void megaraid_sas_kill_hba(struct megasas_instance *instance)
{
	/* Set critical error to block I/O & ioctls in case caller didn't */
	atomic_set(&instance->adprecovery, MEGASAS_HW_CRITICAL_ERROR);
	/* Wait 1 second to ensure IO or ioctls in build have posted */
	msleep(1000);
	if ((instance->pdev->device == PCI_DEVICE_ID_LSI_SAS0073SKINNY) ||
		(instance->pdev->device == PCI_DEVICE_ID_LSI_SAS0071SKINNY) ||
		(instance->adapter_type != MFI_SERIES)) {
		if (!instance->requestorId) {
			writel(MFI_STOP_ADP, &instance->reg_set->doorbell);
			/* Flush */
			readl(&instance->reg_set->doorbell);
		}
		if (instance->requestorId && instance->peerIsPresent)
			memset(instance->ld_ids, 0xff, MEGASAS_MAX_LD_IDS);
	} else {
		writel(MFI_STOP_ADP,
			&instance->reg_set->inbound_doorbell);
	}
	/* Complete outstanding ioctls when adapter is killed */
	megasas_complete_outstanding_ioctls(instance);
}