static void megasas_set_static_target_properties(struct scsi_device *sdev,
						 bool is_target_prop)
{
	u16	target_index = 0;
	u8 interface_type;
	u32 device_qd = MEGASAS_DEFAULT_CMD_PER_LUN;
	u32 max_io_size_kb = MR_DEFAULT_NVME_MDTS_KB;
	u32 tgt_device_qd;
	struct megasas_instance *instance;
	struct MR_PRIV_DEVICE *mr_device_priv_data;

	instance = megasas_lookup_instance(sdev->host->host_no);
	mr_device_priv_data = sdev->hostdata;
	interface_type  = mr_device_priv_data->interface_type;

	/*
	 * The RAID firmware may require extended timeouts.
	 */
	blk_queue_rq_timeout(sdev->request_queue, scmd_timeout * HZ);

	target_index = (sdev->channel * MEGASAS_MAX_DEV_PER_CHANNEL) + sdev->id;

	switch (interface_type) {
	case SAS_PD:
		device_qd = MEGASAS_SAS_QD;
		break;
	case SATA_PD:
		device_qd = MEGASAS_SATA_QD;
		break;
	case NVME_PD:
		device_qd = MEGASAS_NVME_QD;
		break;
	}

	if (is_target_prop) {
		tgt_device_qd = le32_to_cpu(instance->tgt_prop->device_qdepth);
		if (tgt_device_qd &&
		    (tgt_device_qd <= instance->host->can_queue))
			device_qd = tgt_device_qd;

		/* max_io_size_kb will be set to non zero for
		 * nvme based vd and syspd.
		 */
		max_io_size_kb = le32_to_cpu(instance->tgt_prop->max_io_size_kb);
	}

	if (instance->nvme_page_size && max_io_size_kb)
		megasas_set_nvme_device_properties(sdev, (max_io_size_kb << 10));

	scsi_change_queue_depth(sdev, device_qd);

}