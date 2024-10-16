static int megasas_set_crash_dump_params_ioctl(struct megasas_cmd *cmd)
{
	struct megasas_instance *local_instance;
	int i, error = 0;
	int crash_support;

	crash_support = cmd->frame->dcmd.mbox.w[0];

	for (i = 0; i < megasas_mgmt_info.max_index; i++) {
		local_instance = megasas_mgmt_info.instance[i];
		if (local_instance && local_instance->crash_dump_drv_support) {
			if ((atomic_read(&local_instance->adprecovery) ==
				MEGASAS_HBA_OPERATIONAL) &&
				!megasas_set_crash_dump_params(local_instance,
					crash_support)) {
				local_instance->crash_dump_app_support =
					crash_support;
				dev_info(&local_instance->pdev->dev,
					"Application firmware crash "
					"dump mode set success\n");
				error = 0;
			} else {
				dev_info(&local_instance->pdev->dev,
					"Application firmware crash "
					"dump mode set failed\n");
				error = -1;
			}
		}
	}
	return error;
}