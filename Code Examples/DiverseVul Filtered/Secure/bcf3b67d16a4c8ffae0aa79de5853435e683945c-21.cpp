static void megasas_sriov_heartbeat_handler(struct timer_list *t)
{
	struct megasas_instance *instance =
		from_timer(instance, t, sriov_heartbeat_timer);

	if (instance->hb_host_mem->HB.fwCounter !=
	    instance->hb_host_mem->HB.driverCounter) {
		instance->hb_host_mem->HB.driverCounter =
			instance->hb_host_mem->HB.fwCounter;
		mod_timer(&instance->sriov_heartbeat_timer,
			  jiffies + MEGASAS_SRIOV_HEARTBEAT_INTERVAL_VF);
	} else {
		dev_warn(&instance->pdev->dev, "SR-IOV: Heartbeat never "
		       "completed for scsi%d\n", instance->host->host_no);
		schedule_work(&instance->work_init);
	}
}