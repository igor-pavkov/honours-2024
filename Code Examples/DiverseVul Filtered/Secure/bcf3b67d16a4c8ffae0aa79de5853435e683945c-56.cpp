int megasas_set_crash_dump_params(struct megasas_instance *instance,
	u8 crash_buf_state)
{
	int ret = 0;
	struct megasas_cmd *cmd;
	struct megasas_dcmd_frame *dcmd;

	cmd = megasas_get_cmd(instance);

	if (!cmd) {
		dev_err(&instance->pdev->dev, "Failed to get a free cmd\n");
		return -ENOMEM;
	}


	dcmd = &cmd->frame->dcmd;

	memset(dcmd->mbox.b, 0, MFI_MBOX_SIZE);
	dcmd->mbox.b[0] = crash_buf_state;
	dcmd->cmd = MFI_CMD_DCMD;
	dcmd->cmd_status = MFI_STAT_INVALID_STATUS;
	dcmd->sge_count = 1;
	dcmd->flags = MFI_FRAME_DIR_NONE;
	dcmd->timeout = 0;
	dcmd->pad_0 = 0;
	dcmd->data_xfer_len = cpu_to_le32(CRASH_DMA_BUF_SIZE);
	dcmd->opcode = cpu_to_le32(MR_DCMD_CTRL_SET_CRASH_DUMP_PARAMS);

	megasas_set_dma_settings(instance, dcmd, instance->crash_dump_h,
				 CRASH_DMA_BUF_SIZE);

	if ((instance->adapter_type != MFI_SERIES) &&
	    !instance->mask_interrupts)
		ret = megasas_issue_blocked_cmd(instance, cmd, MFI_IO_TIMEOUT_SECS);
	else
		ret = megasas_issue_polled(instance, cmd);

	if (ret == DCMD_TIMEOUT) {
		switch (dcmd_timeout_ocr_possible(instance)) {
		case INITIATE_OCR:
			cmd->flags |= DRV_DCMD_SKIP_REFIRE;
			megasas_reset_fusion(instance->host,
					MFI_IO_TIMEOUT_OCR);
			break;
		case KILL_ADAPTER:
			megaraid_sas_kill_hba(instance);
			break;
		case IGNORE_TIMEOUT:
			dev_info(&instance->pdev->dev, "Ignore DCMD timeout: %s %d\n",
				__func__, __LINE__);
			break;
		}
	} else
		megasas_return_cmd(instance, cmd);

	return ret;
}