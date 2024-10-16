void megasas_add_remove_devices(struct megasas_instance *instance,
				int scan_type)
{
	int i, j;
	u16 pd_index = 0;
	u16 ld_index = 0;
	u16 channel = 0, id = 0;
	struct Scsi_Host *host;
	struct scsi_device *sdev1;
	struct MR_HOST_DEVICE_LIST *targetid_list = NULL;
	struct MR_HOST_DEVICE_LIST_ENTRY *targetid_entry = NULL;

	host = instance->host;

	if (instance->enable_fw_dev_list) {
		targetid_list = instance->host_device_list_buf;
		for (i = 0; i < targetid_list->count; i++) {
			targetid_entry = &targetid_list->host_device_list[i];
			if (targetid_entry->flags.u.bits.is_sys_pd) {
				channel = le16_to_cpu(targetid_entry->target_id) /
						MEGASAS_MAX_DEV_PER_CHANNEL;
				id = le16_to_cpu(targetid_entry->target_id) %
						MEGASAS_MAX_DEV_PER_CHANNEL;
			} else {
				channel = MEGASAS_MAX_PD_CHANNELS +
					  (le16_to_cpu(targetid_entry->target_id) /
					   MEGASAS_MAX_DEV_PER_CHANNEL);
				id = le16_to_cpu(targetid_entry->target_id) %
						MEGASAS_MAX_DEV_PER_CHANNEL;
			}
			sdev1 = scsi_device_lookup(host, channel, id, 0);
			if (!sdev1) {
				scsi_add_device(host, channel, id, 0);
			} else {
				scsi_device_put(sdev1);
			}
		}
	}

	if (scan_type & SCAN_PD_CHANNEL) {
		for (i = 0; i < MEGASAS_MAX_PD_CHANNELS; i++) {
			for (j = 0; j < MEGASAS_MAX_DEV_PER_CHANNEL; j++) {
				pd_index = i * MEGASAS_MAX_DEV_PER_CHANNEL + j;
				sdev1 = scsi_device_lookup(host, i, j, 0);
				if (instance->pd_list[pd_index].driveState ==
							MR_PD_STATE_SYSTEM) {
					if (!sdev1)
						scsi_add_device(host, i, j, 0);
					else
						scsi_device_put(sdev1);
				} else {
					if (sdev1)
						megasas_remove_scsi_device(sdev1);
				}
			}
		}
	}

	if (scan_type & SCAN_VD_CHANNEL) {
		for (i = 0; i < MEGASAS_MAX_LD_CHANNELS; i++) {
			for (j = 0; j < MEGASAS_MAX_DEV_PER_CHANNEL; j++) {
				ld_index = (i * MEGASAS_MAX_DEV_PER_CHANNEL) + j;
				sdev1 = scsi_device_lookup(host,
						MEGASAS_MAX_PD_CHANNELS + i, j, 0);
				if (instance->ld_ids[ld_index] != 0xff) {
					if (!sdev1)
						scsi_add_device(host, MEGASAS_MAX_PD_CHANNELS + i, j, 0);
					else
						scsi_device_put(sdev1);
				} else {
					if (sdev1)
						megasas_remove_scsi_device(sdev1);
				}
			}
		}
	}

}