static inline void megasas_init_ctrl_params(struct megasas_instance *instance)
{
	instance->fw_crash_state = UNAVAILABLE;

	megasas_poll_wait_aen = 0;
	instance->issuepend_done = 1;
	atomic_set(&instance->adprecovery, MEGASAS_HBA_OPERATIONAL);

	/*
	 * Initialize locks and queues
	 */
	INIT_LIST_HEAD(&instance->cmd_pool);
	INIT_LIST_HEAD(&instance->internal_reset_pending_q);

	atomic_set(&instance->fw_outstanding, 0);

	init_waitqueue_head(&instance->int_cmd_wait_q);
	init_waitqueue_head(&instance->abort_cmd_wait_q);

	spin_lock_init(&instance->crashdump_lock);
	spin_lock_init(&instance->mfi_pool_lock);
	spin_lock_init(&instance->hba_lock);
	spin_lock_init(&instance->stream_lock);
	spin_lock_init(&instance->completion_lock);

	mutex_init(&instance->reset_mutex);

	if ((instance->pdev->device == PCI_DEVICE_ID_LSI_SAS0073SKINNY) ||
	    (instance->pdev->device == PCI_DEVICE_ID_LSI_SAS0071SKINNY))
		instance->flag_ieee = 1;

	megasas_dbg_lvl = 0;
	instance->flag = 0;
	instance->unload = 1;
	instance->last_time = 0;
	instance->disableOnlineCtrlReset = 1;
	instance->UnevenSpanSupport = 0;

	if (instance->adapter_type != MFI_SERIES)
		INIT_WORK(&instance->work_init, megasas_fusion_ocr_wq);
	else
		INIT_WORK(&instance->work_init, process_fw_state_change_wq);
}