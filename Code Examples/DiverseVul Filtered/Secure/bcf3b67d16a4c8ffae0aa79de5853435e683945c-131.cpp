static int megasas_get_ld_vf_affiliation_111(struct megasas_instance *instance,
					    int initial)
{
	struct megasas_cmd *cmd;
	struct megasas_dcmd_frame *dcmd;
	struct MR_LD_VF_AFFILIATION_111 *new_affiliation_111 = NULL;
	dma_addr_t new_affiliation_111_h;
	int ld, retval = 0;
	u8 thisVf;

	cmd = megasas_get_cmd(instance);

	if (!cmd) {
		dev_printk(KERN_DEBUG, &instance->pdev->dev, "megasas_get_ld_vf_affiliation_111:"
		       "Failed to get cmd for scsi%d\n",
			instance->host->host_no);
		return -ENOMEM;
	}

	dcmd = &cmd->frame->dcmd;

	if (!instance->vf_affiliation_111) {
		dev_warn(&instance->pdev->dev, "SR-IOV: Couldn't get LD/VF "
		       "affiliation for scsi%d\n", instance->host->host_no);
		megasas_return_cmd(instance, cmd);
		return -ENOMEM;
	}

	if (initial)
			memset(instance->vf_affiliation_111, 0,
			       sizeof(struct MR_LD_VF_AFFILIATION_111));
	else {
		new_affiliation_111 =
			dma_zalloc_coherent(&instance->pdev->dev,
					      sizeof(struct MR_LD_VF_AFFILIATION_111),
					      &new_affiliation_111_h, GFP_KERNEL);
		if (!new_affiliation_111) {
			dev_printk(KERN_DEBUG, &instance->pdev->dev, "SR-IOV: Couldn't allocate "
			       "memory for new affiliation for scsi%d\n",
			       instance->host->host_no);
			megasas_return_cmd(instance, cmd);
			return -ENOMEM;
		}
	}

	memset(dcmd->mbox.b, 0, MFI_MBOX_SIZE);

	dcmd->cmd = MFI_CMD_DCMD;
	dcmd->cmd_status = MFI_STAT_INVALID_STATUS;
	dcmd->sge_count = 1;
	dcmd->flags = cpu_to_le16(MFI_FRAME_DIR_BOTH);
	dcmd->timeout = 0;
	dcmd->pad_0 = 0;
	dcmd->data_xfer_len =
		cpu_to_le32(sizeof(struct MR_LD_VF_AFFILIATION_111));
	dcmd->opcode = cpu_to_le32(MR_DCMD_LD_VF_MAP_GET_ALL_LDS_111);

	if (initial)
		dcmd->sgl.sge32[0].phys_addr =
			cpu_to_le32(instance->vf_affiliation_111_h);
	else
		dcmd->sgl.sge32[0].phys_addr =
			cpu_to_le32(new_affiliation_111_h);

	dcmd->sgl.sge32[0].length = cpu_to_le32(
		sizeof(struct MR_LD_VF_AFFILIATION_111));

	dev_warn(&instance->pdev->dev, "SR-IOV: Getting LD/VF affiliation for "
	       "scsi%d\n", instance->host->host_no);

	if (megasas_issue_blocked_cmd(instance, cmd, 0) != DCMD_SUCCESS) {
		dev_warn(&instance->pdev->dev, "SR-IOV: LD/VF affiliation DCMD"
		       " failed with status 0x%x for scsi%d\n",
		       dcmd->cmd_status, instance->host->host_no);
		retval = 1; /* Do a scan if we couldn't get affiliation */
		goto out;
	}

	if (!initial) {
		thisVf = new_affiliation_111->thisVf;
		for (ld = 0 ; ld < new_affiliation_111->vdCount; ld++)
			if (instance->vf_affiliation_111->map[ld].policy[thisVf] !=
			    new_affiliation_111->map[ld].policy[thisVf]) {
				dev_warn(&instance->pdev->dev, "SR-IOV: "
				       "Got new LD/VF affiliation for scsi%d\n",
				       instance->host->host_no);
				memcpy(instance->vf_affiliation_111,
				       new_affiliation_111,
				       sizeof(struct MR_LD_VF_AFFILIATION_111));
				retval = 1;
				goto out;
			}
	}
out:
	if (new_affiliation_111) {
		dma_free_coherent(&instance->pdev->dev,
				    sizeof(struct MR_LD_VF_AFFILIATION_111),
				    new_affiliation_111,
				    new_affiliation_111_h);
	}

	megasas_return_cmd(instance, cmd);

	return retval;
}