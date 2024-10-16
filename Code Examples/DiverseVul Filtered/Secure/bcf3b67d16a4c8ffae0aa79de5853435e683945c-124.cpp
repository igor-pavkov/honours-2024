void megasas_get_snapdump_properties(struct megasas_instance *instance)
{
	int ret = 0;
	struct megasas_cmd *cmd;
	struct megasas_dcmd_frame *dcmd;
	struct MR_SNAPDUMP_PROPERTIES *ci;
	dma_addr_t ci_h = 0;

	ci = instance->snapdump_prop;
	ci_h = instance->snapdump_prop_h;

	if (!ci)
		return;

	cmd = megasas_get_cmd(instance);

	if (!cmd) {
		dev_dbg(&instance->pdev->dev, "Failed to get a free cmd\n");
		return;
	}

	dcmd = &cmd->frame->dcmd;

	memset(ci, 0, sizeof(*ci));
	memset(dcmd->mbox.b, 0, MFI_MBOX_SIZE);

	dcmd->cmd = MFI_CMD_DCMD;
	dcmd->cmd_status = MFI_STAT_INVALID_STATUS;
	dcmd->sge_count = 1;
	dcmd->flags = MFI_FRAME_DIR_READ;
	dcmd->timeout = 0;
	dcmd->pad_0 = 0;
	dcmd->data_xfer_len = cpu_to_le32(sizeof(struct MR_SNAPDUMP_PROPERTIES));
	dcmd->opcode = cpu_to_le32(MR_DCMD_CTRL_SNAPDUMP_GET_PROPERTIES);

	megasas_set_dma_settings(instance, dcmd, ci_h,
				 sizeof(struct MR_SNAPDUMP_PROPERTIES));

	if (!instance->mask_interrupts) {
		ret = megasas_issue_blocked_cmd(instance, cmd,
						MFI_IO_TIMEOUT_SECS);
	} else {
		ret = megasas_issue_polled(instance, cmd);
		cmd->flags |= DRV_DCMD_SKIP_REFIRE;
	}

	switch (ret) {
	case DCMD_SUCCESS:
		instance->snapdump_wait_time =
			min_t(u8, ci->trigger_min_num_sec_before_ocr,
				MEGASAS_MAX_SNAP_DUMP_WAIT_TIME);
		break;

	case DCMD_TIMEOUT:
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
	}

	if (ret != DCMD_TIMEOUT)
		megasas_return_cmd(instance, cmd);
}