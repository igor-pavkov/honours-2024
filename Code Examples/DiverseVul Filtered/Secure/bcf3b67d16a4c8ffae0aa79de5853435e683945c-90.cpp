static int megasas_slave_alloc(struct scsi_device *sdev)
{
	u16 pd_index = 0;
	struct megasas_instance *instance ;
	struct MR_PRIV_DEVICE *mr_device_priv_data;

	instance = megasas_lookup_instance(sdev->host->host_no);
	if (!MEGASAS_IS_LOGICAL(sdev)) {
		/*
		 * Open the OS scan to the SYSTEM PD
		 */
		pd_index =
			(sdev->channel * MEGASAS_MAX_DEV_PER_CHANNEL) +
			sdev->id;
		if ((instance->pd_list_not_supported ||
			instance->pd_list[pd_index].driveState ==
			MR_PD_STATE_SYSTEM)) {
			goto scan_target;
		}
		return -ENXIO;
	}

scan_target:
	mr_device_priv_data = kzalloc(sizeof(*mr_device_priv_data),
					GFP_KERNEL);
	if (!mr_device_priv_data)
		return -ENOMEM;
	sdev->hostdata = mr_device_priv_data;

	atomic_set(&mr_device_priv_data->r1_ldio_hint,
		   instance->r1_ldio_hint_default);
	return 0;
}