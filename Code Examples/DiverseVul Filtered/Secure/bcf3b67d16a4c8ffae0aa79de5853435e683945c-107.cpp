static void megasas_complete_outstanding_ioctls(struct megasas_instance *instance)
{
	int i;
	struct megasas_cmd *cmd_mfi;
	struct megasas_cmd_fusion *cmd_fusion;
	struct fusion_context *fusion = instance->ctrl_context;

	/* Find all outstanding ioctls */
	if (fusion) {
		for (i = 0; i < instance->max_fw_cmds; i++) {
			cmd_fusion = fusion->cmd_list[i];
			if (cmd_fusion->sync_cmd_idx != (u32)ULONG_MAX) {
				cmd_mfi = instance->cmd_list[cmd_fusion->sync_cmd_idx];
				if (cmd_mfi->sync_cmd &&
				    (cmd_mfi->frame->hdr.cmd != MFI_CMD_ABORT)) {
					cmd_mfi->frame->hdr.cmd_status =
							MFI_STAT_WRONG_STATE;
					megasas_complete_cmd(instance,
							     cmd_mfi, DID_OK);
				}
			}
		}
	} else {
		for (i = 0; i < instance->max_fw_cmds; i++) {
			cmd_mfi = instance->cmd_list[i];
			if (cmd_mfi->sync_cmd && cmd_mfi->frame->hdr.cmd !=
				MFI_CMD_ABORT)
				megasas_complete_cmd(instance, cmd_mfi, DID_OK);
		}
	}
}