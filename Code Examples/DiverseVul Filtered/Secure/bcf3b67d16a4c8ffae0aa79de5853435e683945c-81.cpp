static int megasas_get_ld_vf_affiliation_12(struct megasas_instance *instance,
					    int initial)
{
	struct megasas_cmd *cmd;
	struct megasas_dcmd_frame *dcmd;
	struct MR_LD_VF_AFFILIATION *new_affiliation = NULL;
	struct MR_LD_VF_MAP *newmap = NULL, *savedmap = NULL;
	dma_addr_t new_affiliation_h;
	int i, j, retval = 0, found = 0, doscan = 0;
	u8 thisVf;

	cmd = megasas_get_cmd(instance);

	if (!cmd) {
		dev_printk(KERN_DEBUG, &instance->pdev->dev, "megasas_get_ld_vf_affiliation12: "
		       "Failed to get cmd for scsi%d\n",
		       instance->host->host_no);
		return -ENOMEM;
	}

	dcmd = &cmd->frame->dcmd;

	if (!instance->vf_affiliation) {
		dev_warn(&instance->pdev->dev, "SR-IOV: Couldn't get LD/VF "
		       "affiliation for scsi%d\n", instance->host->host_no);
		megasas_return_cmd(instance, cmd);
		return -ENOMEM;
	}

	if (initial)
		memset(instance->vf_affiliation, 0, (MAX_LOGICAL_DRIVES + 1) *
		       sizeof(struct MR_LD_VF_AFFILIATION));
	else {
		new_affiliation =
			dma_zalloc_coherent(&instance->pdev->dev,
					      (MAX_LOGICAL_DRIVES + 1) *
					      sizeof(struct MR_LD_VF_AFFILIATION),
					      &new_affiliation_h, GFP_KERNEL);
		if (!new_affiliation) {
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
	dcmd->data_xfer_len = cpu_to_le32((MAX_LOGICAL_DRIVES + 1) *
		sizeof(struct MR_LD_VF_AFFILIATION));
	dcmd->opcode = cpu_to_le32(MR_DCMD_LD_VF_MAP_GET_ALL_LDS);

	if (initial)
		dcmd->sgl.sge32[0].phys_addr =
			cpu_to_le32(instance->vf_affiliation_h);
	else
		dcmd->sgl.sge32[0].phys_addr =
			cpu_to_le32(new_affiliation_h);

	dcmd->sgl.sge32[0].length = cpu_to_le32((MAX_LOGICAL_DRIVES + 1) *
		sizeof(struct MR_LD_VF_AFFILIATION));

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
		if (!new_affiliation->ldCount) {
			dev_warn(&instance->pdev->dev, "SR-IOV: Got new LD/VF "
			       "affiliation for passive path for scsi%d\n",
			       instance->host->host_no);
			retval = 1;
			goto out;
		}
		newmap = new_affiliation->map;
		savedmap = instance->vf_affiliation->map;
		thisVf = new_affiliation->thisVf;
		for (i = 0 ; i < new_affiliation->ldCount; i++) {
			found = 0;
			for (j = 0; j < instance->vf_affiliation->ldCount;
			     j++) {
				if (newmap->ref.targetId ==
				    savedmap->ref.targetId) {
					found = 1;
					if (newmap->policy[thisVf] !=
					    savedmap->policy[thisVf]) {
						doscan = 1;
						goto out;
					}
				}
				savedmap = (struct MR_LD_VF_MAP *)
					((unsigned char *)savedmap +
					 savedmap->size);
			}
			if (!found && newmap->policy[thisVf] !=
			    MR_LD_ACCESS_HIDDEN) {
				doscan = 1;
				goto out;
			}
			newmap = (struct MR_LD_VF_MAP *)
				((unsigned char *)newmap + newmap->size);
		}

		newmap = new_affiliation->map;
		savedmap = instance->vf_affiliation->map;

		for (i = 0 ; i < instance->vf_affiliation->ldCount; i++) {
			found = 0;
			for (j = 0 ; j < new_affiliation->ldCount; j++) {
				if (savedmap->ref.targetId ==
				    newmap->ref.targetId) {
					found = 1;
					if (savedmap->policy[thisVf] !=
					    newmap->policy[thisVf]) {
						doscan = 1;
						goto out;
					}
				}
				newmap = (struct MR_LD_VF_MAP *)
					((unsigned char *)newmap +
					 newmap->size);
			}
			if (!found && savedmap->policy[thisVf] !=
			    MR_LD_ACCESS_HIDDEN) {
				doscan = 1;
				goto out;
			}
			savedmap = (struct MR_LD_VF_MAP *)
				((unsigned char *)savedmap +
				 savedmap->size);
		}
	}
out:
	if (doscan) {
		dev_warn(&instance->pdev->dev, "SR-IOV: Got new LD/VF "
		       "affiliation for scsi%d\n", instance->host->host_no);
		memcpy(instance->vf_affiliation, new_affiliation,
		       new_affiliation->size);
		retval = 1;
	}

	if (new_affiliation)
		dma_free_coherent(&instance->pdev->dev,
				    (MAX_LOGICAL_DRIVES + 1) *
				    sizeof(struct MR_LD_VF_AFFILIATION),
				    new_affiliation, new_affiliation_h);
	megasas_return_cmd(instance, cmd);

	return retval;
}