static int megasas_wait_for_outstanding(struct megasas_instance *instance)
{
	int i, sl, outstanding;
	u32 reset_index;
	u32 wait_time = MEGASAS_RESET_WAIT_TIME;
	unsigned long flags;
	struct list_head clist_local;
	struct megasas_cmd *reset_cmd;
	u32 fw_state;

	if (atomic_read(&instance->adprecovery) == MEGASAS_HW_CRITICAL_ERROR) {
		dev_info(&instance->pdev->dev, "%s:%d HBA is killed.\n",
		__func__, __LINE__);
		return FAILED;
	}

	if (atomic_read(&instance->adprecovery) != MEGASAS_HBA_OPERATIONAL) {

		INIT_LIST_HEAD(&clist_local);
		spin_lock_irqsave(&instance->hba_lock, flags);
		list_splice_init(&instance->internal_reset_pending_q,
				&clist_local);
		spin_unlock_irqrestore(&instance->hba_lock, flags);

		dev_notice(&instance->pdev->dev, "HBA reset wait ...\n");
		for (i = 0; i < wait_time; i++) {
			msleep(1000);
			if (atomic_read(&instance->adprecovery) == MEGASAS_HBA_OPERATIONAL)
				break;
		}

		if (atomic_read(&instance->adprecovery) != MEGASAS_HBA_OPERATIONAL) {
			dev_notice(&instance->pdev->dev, "reset: Stopping HBA.\n");
			atomic_set(&instance->adprecovery, MEGASAS_HW_CRITICAL_ERROR);
			return FAILED;
		}

		reset_index = 0;
		while (!list_empty(&clist_local)) {
			reset_cmd = list_entry((&clist_local)->next,
						struct megasas_cmd, list);
			list_del_init(&reset_cmd->list);
			if (reset_cmd->scmd) {
				reset_cmd->scmd->result = DID_REQUEUE << 16;
				dev_notice(&instance->pdev->dev, "%d:%p reset [%02x]\n",
					reset_index, reset_cmd,
					reset_cmd->scmd->cmnd[0]);

				reset_cmd->scmd->scsi_done(reset_cmd->scmd);
				megasas_return_cmd(instance, reset_cmd);
			} else if (reset_cmd->sync_cmd) {
				dev_notice(&instance->pdev->dev, "%p synch cmds"
						"reset queue\n",
						reset_cmd);

				reset_cmd->cmd_status_drv = MFI_STAT_INVALID_STATUS;
				instance->instancet->fire_cmd(instance,
						reset_cmd->frame_phys_addr,
						0, instance->reg_set);
			} else {
				dev_notice(&instance->pdev->dev, "%p unexpected"
					"cmds lst\n",
					reset_cmd);
			}
			reset_index++;
		}

		return SUCCESS;
	}

	for (i = 0; i < resetwaittime; i++) {
		outstanding = atomic_read(&instance->fw_outstanding);

		if (!outstanding)
			break;

		if (!(i % MEGASAS_RESET_NOTICE_INTERVAL)) {
			dev_notice(&instance->pdev->dev, "[%2d]waiting for %d "
			       "commands to complete\n",i,outstanding);
			/*
			 * Call cmd completion routine. Cmd to be
			 * be completed directly without depending on isr.
			 */
			megasas_complete_cmd_dpc((unsigned long)instance);
		}

		msleep(1000);
	}

	i = 0;
	outstanding = atomic_read(&instance->fw_outstanding);
	fw_state = instance->instancet->read_fw_status_reg(instance) & MFI_STATE_MASK;

	if ((!outstanding && (fw_state == MFI_STATE_OPERATIONAL)))
		goto no_outstanding;

	if (instance->disableOnlineCtrlReset)
		goto kill_hba_and_failed;
	do {
		if ((fw_state == MFI_STATE_FAULT) || atomic_read(&instance->fw_outstanding)) {
			dev_info(&instance->pdev->dev,
				"%s:%d waiting_for_outstanding: before issue OCR. FW state = 0x%x, oustanding 0x%x\n",
				__func__, __LINE__, fw_state, atomic_read(&instance->fw_outstanding));
			if (i == 3)
				goto kill_hba_and_failed;
			megasas_do_ocr(instance);

			if (atomic_read(&instance->adprecovery) == MEGASAS_HW_CRITICAL_ERROR) {
				dev_info(&instance->pdev->dev, "%s:%d OCR failed and HBA is killed.\n",
				__func__, __LINE__);
				return FAILED;
			}
			dev_info(&instance->pdev->dev, "%s:%d waiting_for_outstanding: after issue OCR.\n",
				__func__, __LINE__);

			for (sl = 0; sl < 10; sl++)
				msleep(500);

			outstanding = atomic_read(&instance->fw_outstanding);

			fw_state = instance->instancet->read_fw_status_reg(instance) & MFI_STATE_MASK;
			if ((!outstanding && (fw_state == MFI_STATE_OPERATIONAL)))
				goto no_outstanding;
		}
		i++;
	} while (i <= 3);

no_outstanding:

	dev_info(&instance->pdev->dev, "%s:%d no more pending commands remain after reset handling.\n",
		__func__, __LINE__);
	return SUCCESS;

kill_hba_and_failed:

	/* Reset not supported, kill adapter */
	dev_info(&instance->pdev->dev, "%s:%d killing adapter scsi%d"
		" disableOnlineCtrlReset %d fw_outstanding %d \n",
		__func__, __LINE__, instance->host->host_no, instance->disableOnlineCtrlReset,
		atomic_read(&instance->fw_outstanding));
	megasas_dump_pending_frames(instance);
	megaraid_sas_kill_hba(instance);

	return FAILED;
}