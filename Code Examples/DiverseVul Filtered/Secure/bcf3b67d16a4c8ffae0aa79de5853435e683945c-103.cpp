static void megasas_update_ext_vd_details(struct megasas_instance *instance)
{
	struct fusion_context *fusion;
	u32 ventura_map_sz = 0;

	fusion = instance->ctrl_context;
	/* For MFI based controllers return dummy success */
	if (!fusion)
		return;

	instance->supportmax256vd =
		instance->ctrl_info_buf->adapterOperations3.supportMaxExtLDs;
	/* Below is additional check to address future FW enhancement */
	if (instance->ctrl_info_buf->max_lds > 64)
		instance->supportmax256vd = 1;

	instance->drv_supported_vd_count = MEGASAS_MAX_LD_CHANNELS
					* MEGASAS_MAX_DEV_PER_CHANNEL;
	instance->drv_supported_pd_count = MEGASAS_MAX_PD_CHANNELS
					* MEGASAS_MAX_DEV_PER_CHANNEL;
	if (instance->supportmax256vd) {
		instance->fw_supported_vd_count = MAX_LOGICAL_DRIVES_EXT;
		instance->fw_supported_pd_count = MAX_PHYSICAL_DEVICES;
	} else {
		instance->fw_supported_vd_count = MAX_LOGICAL_DRIVES;
		instance->fw_supported_pd_count = MAX_PHYSICAL_DEVICES;
	}

	dev_info(&instance->pdev->dev,
		"FW provided supportMaxExtLDs: %d\tmax_lds: %d\n",
		instance->ctrl_info_buf->adapterOperations3.supportMaxExtLDs ? 1 : 0,
		instance->ctrl_info_buf->max_lds);

	if (instance->max_raid_mapsize) {
		ventura_map_sz = instance->max_raid_mapsize *
						MR_MIN_MAP_SIZE; /* 64k */
		fusion->current_map_sz = ventura_map_sz;
		fusion->max_map_sz = ventura_map_sz;
	} else {
		fusion->old_map_sz =  sizeof(struct MR_FW_RAID_MAP) +
					(sizeof(struct MR_LD_SPAN_MAP) *
					(instance->fw_supported_vd_count - 1));
		fusion->new_map_sz =  sizeof(struct MR_FW_RAID_MAP_EXT);

		fusion->max_map_sz =
			max(fusion->old_map_sz, fusion->new_map_sz);

		if (instance->supportmax256vd)
			fusion->current_map_sz = fusion->new_map_sz;
		else
			fusion->current_map_sz = fusion->old_map_sz;
	}
	/* irrespective of FW raid maps, driver raid map is constant */
	fusion->drv_map_sz = sizeof(struct MR_DRV_RAID_MAP_ALL);
}