static int megasas_task_abort(struct scsi_cmnd *scmd)
{
	int ret;
	struct megasas_instance *instance;

	instance = (struct megasas_instance *)scmd->device->host->hostdata;

	if (instance->adapter_type != MFI_SERIES)
		ret = megasas_task_abort_fusion(scmd);
	else {
		sdev_printk(KERN_NOTICE, scmd->device, "TASK ABORT not supported\n");
		ret = FAILED;
	}

	return ret;
}