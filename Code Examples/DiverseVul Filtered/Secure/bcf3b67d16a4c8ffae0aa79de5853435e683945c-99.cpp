int megasas_get_device_list(struct megasas_instance *instance)
{
	memset(instance->pd_list, 0,
	       (MEGASAS_MAX_PD * sizeof(struct megasas_pd_list)));
	memset(instance->ld_ids, 0xff, MEGASAS_MAX_LD_IDS);

	if (instance->enable_fw_dev_list) {
		if (megasas_host_device_list_query(instance, true))
			return FAILED;
	} else {
		if (megasas_get_pd_list(instance) < 0) {
			dev_err(&instance->pdev->dev, "failed to get PD list\n");
			return FAILED;
		}

		if (megasas_ld_list_query(instance,
					  MR_LD_QUERY_TYPE_EXPOSED_TO_HOST)) {
			dev_err(&instance->pdev->dev, "failed to get LD list\n");
			return FAILED;
		}
	}

	return SUCCESS;
}