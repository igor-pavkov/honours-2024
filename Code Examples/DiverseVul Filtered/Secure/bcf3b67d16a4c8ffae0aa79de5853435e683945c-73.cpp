void megasas_set_dynamic_target_properties(struct scsi_device *sdev,
					   bool is_target_prop)
{
	u16 pd_index = 0, ld;
	u32 device_id;
	struct megasas_instance *instance;
	struct fusion_context *fusion;
	struct MR_PRIV_DEVICE *mr_device_priv_data;
	struct MR_PD_CFG_SEQ_NUM_SYNC *pd_sync;
	struct MR_LD_RAID *raid;
	struct MR_DRV_RAID_MAP_ALL *local_map_ptr;

	instance = megasas_lookup_instance(sdev->host->host_no);
	fusion = instance->ctrl_context;
	mr_device_priv_data = sdev->hostdata;

	if (!fusion || !mr_device_priv_data)
		return;

	if (MEGASAS_IS_LOGICAL(sdev)) {
		device_id = ((sdev->channel % 2) * MEGASAS_MAX_DEV_PER_CHANNEL)
					+ sdev->id;
		local_map_ptr = fusion->ld_drv_map[(instance->map_id & 1)];
		ld = MR_TargetIdToLdGet(device_id, local_map_ptr);
		if (ld >= instance->fw_supported_vd_count)
			return;
		raid = MR_LdRaidGet(ld, local_map_ptr);

		if (raid->capability.ldPiMode == MR_PROT_INFO_TYPE_CONTROLLER)
		blk_queue_update_dma_alignment(sdev->request_queue, 0x7);

		mr_device_priv_data->is_tm_capable =
			raid->capability.tmCapable;
	} else if (instance->use_seqnum_jbod_fp) {
		pd_index = (sdev->channel * MEGASAS_MAX_DEV_PER_CHANNEL) +
			sdev->id;
		pd_sync = (void *)fusion->pd_seq_sync
				[(instance->pd_seq_map_id - 1) & 1];
		mr_device_priv_data->is_tm_capable =
			pd_sync->seq[pd_index].capability.tmCapable;
	}

	if (is_target_prop && instance->tgt_prop->reset_tmo) {
		/*
		 * If FW provides a target reset timeout value, driver will use
		 * it. If not set, fallback to default values.
		 */
		mr_device_priv_data->target_reset_tmo =
			min_t(u8, instance->max_reset_tmo,
			      instance->tgt_prop->reset_tmo);
		mr_device_priv_data->task_abort_tmo = instance->task_abort_tmo;
	} else {
		mr_device_priv_data->target_reset_tmo =
						MEGASAS_DEFAULT_TM_TIMEOUT;
		mr_device_priv_data->task_abort_tmo =
						MEGASAS_DEFAULT_TM_TIMEOUT;
	}
}