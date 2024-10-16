static int megasas_slave_configure(struct scsi_device *sdev)
{
	u16 pd_index = 0;
	struct megasas_instance *instance;
	int ret_target_prop = DCMD_FAILED;
	bool is_target_prop = false;

	instance = megasas_lookup_instance(sdev->host->host_no);
	if (instance->pd_list_not_supported) {
		if (!MEGASAS_IS_LOGICAL(sdev) && sdev->type == TYPE_DISK) {
			pd_index = (sdev->channel * MEGASAS_MAX_DEV_PER_CHANNEL) +
				sdev->id;
			if (instance->pd_list[pd_index].driveState !=
				MR_PD_STATE_SYSTEM)
				return -ENXIO;
		}
	}

	mutex_lock(&instance->reset_mutex);
	/* Send DCMD to Firmware and cache the information */
	if ((instance->pd_info) && !MEGASAS_IS_LOGICAL(sdev))
		megasas_get_pd_info(instance, sdev);

	/* Some ventura firmware may not have instance->nvme_page_size set.
	 * Do not send MR_DCMD_DRV_GET_TARGET_PROP
	 */
	if ((instance->tgt_prop) && (instance->nvme_page_size))
		ret_target_prop = megasas_get_target_prop(instance, sdev);

	is_target_prop = (ret_target_prop == DCMD_SUCCESS) ? true : false;
	megasas_set_static_target_properties(sdev, is_target_prop);

	/* This sdev property may change post OCR */
	megasas_set_dynamic_target_properties(sdev, is_target_prop);

	mutex_unlock(&instance->reset_mutex);

	return 0;
}