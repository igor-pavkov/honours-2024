static int megasas_io_attach(struct megasas_instance *instance)
{
	struct Scsi_Host *host = instance->host;

	/*
	 * Export parameters required by SCSI mid-layer
	 */
	host->unique_id = instance->unique_id;
	host->can_queue = instance->max_scsi_cmds;
	host->this_id = instance->init_id;
	host->sg_tablesize = instance->max_num_sge;

	if (instance->fw_support_ieee)
		instance->max_sectors_per_req = MEGASAS_MAX_SECTORS_IEEE;

	/*
	 * Check if the module parameter value for max_sectors can be used
	 */
	if (max_sectors && max_sectors < instance->max_sectors_per_req)
		instance->max_sectors_per_req = max_sectors;
	else {
		if (max_sectors) {
			if (((instance->pdev->device ==
				PCI_DEVICE_ID_LSI_SAS1078GEN2) ||
				(instance->pdev->device ==
				PCI_DEVICE_ID_LSI_SAS0079GEN2)) &&
				(max_sectors <= MEGASAS_MAX_SECTORS)) {
				instance->max_sectors_per_req = max_sectors;
			} else {
			dev_info(&instance->pdev->dev, "max_sectors should be > 0"
				"and <= %d (or < 1MB for GEN2 controller)\n",
				instance->max_sectors_per_req);
			}
		}
	}

	host->max_sectors = instance->max_sectors_per_req;
	host->cmd_per_lun = MEGASAS_DEFAULT_CMD_PER_LUN;
	host->max_channel = MEGASAS_MAX_CHANNELS - 1;
	host->max_id = MEGASAS_MAX_DEV_PER_CHANNEL;
	host->max_lun = MEGASAS_MAX_LUN;
	host->max_cmd_len = 16;

	/*
	 * Notify the mid-layer about the new controller
	 */
	if (scsi_add_host(host, &instance->pdev->dev)) {
		dev_err(&instance->pdev->dev,
			"Failed to add host from %s %d\n",
			__func__, __LINE__);
		return -ENODEV;
	}

	return 0;
}