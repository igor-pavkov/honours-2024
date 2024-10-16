static void megasas_shutdown_controller(struct megasas_instance *instance,
					u32 opcode)
{
	struct megasas_cmd *cmd;
	struct megasas_dcmd_frame *dcmd;

	if (atomic_read(&instance->adprecovery) == MEGASAS_HW_CRITICAL_ERROR)
		return;

	cmd = megasas_get_cmd(instance);

	if (!cmd)
		return;

	if (instance->aen_cmd)
		megasas_issue_blocked_abort_cmd(instance,
			instance->aen_cmd, MFI_IO_TIMEOUT_SECS);
	if (instance->map_update_cmd)
		megasas_issue_blocked_abort_cmd(instance,
			instance->map_update_cmd, MFI_IO_TIMEOUT_SECS);
	if (instance->jbod_seq_cmd)
		megasas_issue_blocked_abort_cmd(instance,
			instance->jbod_seq_cmd, MFI_IO_TIMEOUT_SECS);

	dcmd = &cmd->frame->dcmd;

	memset(dcmd->mbox.b, 0, MFI_MBOX_SIZE);

	dcmd->cmd = MFI_CMD_DCMD;
	dcmd->cmd_status = 0x0;
	dcmd->sge_count = 0;
	dcmd->flags = cpu_to_le16(MFI_FRAME_DIR_NONE);
	dcmd->timeout = 0;
	dcmd->pad_0 = 0;
	dcmd->data_xfer_len = 0;
	dcmd->opcode = cpu_to_le32(opcode);

	if (megasas_issue_blocked_cmd(instance, cmd, MFI_IO_TIMEOUT_SECS)
			!= DCMD_SUCCESS) {
		dev_err(&instance->pdev->dev,
			"return from %s %d\n", __func__, __LINE__);
		return;
	}

	megasas_return_cmd(instance, cmd);
}