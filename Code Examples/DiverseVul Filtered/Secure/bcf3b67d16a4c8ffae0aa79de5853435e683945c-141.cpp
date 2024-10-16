int megasas_sriov_start_heartbeat(struct megasas_instance *instance,
					 int initial)
{
	struct megasas_cmd *cmd;
	struct megasas_dcmd_frame *dcmd;
	int retval = 0;

	cmd = megasas_get_cmd(instance);

	if (!cmd) {
		dev_printk(KERN_DEBUG, &instance->pdev->dev, "megasas_sriov_start_heartbeat: "
		       "Failed to get cmd for scsi%d\n",
		       instance->host->host_no);
		return -ENOMEM;
	}

	dcmd = &cmd->frame->dcmd;

	if (initial) {
		instance->hb_host_mem =
			dma_zalloc_coherent(&instance->pdev->dev,
					      sizeof(struct MR_CTRL_HB_HOST_MEM),
					      &instance->hb_host_mem_h, GFP_KERNEL);
		if (!instance->hb_host_mem) {
			dev_printk(KERN_DEBUG, &instance->pdev->dev, "SR-IOV: Couldn't allocate"
			       " memory for heartbeat host memory for scsi%d\n",
			       instance->host->host_no);
			retval = -ENOMEM;
			goto out;
		}
	}

	memset(dcmd->mbox.b, 0, MFI_MBOX_SIZE);

	dcmd->mbox.s[0] = cpu_to_le16(sizeof(struct MR_CTRL_HB_HOST_MEM));
	dcmd->cmd = MFI_CMD_DCMD;
	dcmd->cmd_status = MFI_STAT_INVALID_STATUS;
	dcmd->sge_count = 1;
	dcmd->flags = cpu_to_le16(MFI_FRAME_DIR_BOTH);
	dcmd->timeout = 0;
	dcmd->pad_0 = 0;
	dcmd->data_xfer_len = cpu_to_le32(sizeof(struct MR_CTRL_HB_HOST_MEM));
	dcmd->opcode = cpu_to_le32(MR_DCMD_CTRL_SHARED_HOST_MEM_ALLOC);

	megasas_set_dma_settings(instance, dcmd, instance->hb_host_mem_h,
				 sizeof(struct MR_CTRL_HB_HOST_MEM));

	dev_warn(&instance->pdev->dev, "SR-IOV: Starting heartbeat for scsi%d\n",
	       instance->host->host_no);

	if ((instance->adapter_type != MFI_SERIES) &&
	    !instance->mask_interrupts)
		retval = megasas_issue_blocked_cmd(instance, cmd,
			MEGASAS_ROUTINE_WAIT_TIME_VF);
	else
		retval = megasas_issue_polled(instance, cmd);

	if (retval) {
		dev_warn(&instance->pdev->dev, "SR-IOV: MR_DCMD_CTRL_SHARED_HOST"
			"_MEM_ALLOC DCMD %s for scsi%d\n",
			(dcmd->cmd_status == MFI_STAT_INVALID_STATUS) ?
			"timed out" : "failed", instance->host->host_no);
		retval = 1;
	}

out:
	megasas_return_cmd(instance, cmd);

	return retval;
}