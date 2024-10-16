static int megasas_reset_target(struct scsi_cmnd *scmd)
{
	int ret;
	struct megasas_instance *instance;

	instance = (struct megasas_instance *)scmd->device->host->hostdata;

	if (instance->adapter_type != MFI_SERIES)
		ret = megasas_reset_target_fusion(scmd);
	else {
		sdev_printk(KERN_NOTICE, scmd->device, "TARGET RESET not supported\n");
		ret = FAILED;
	}

	return ret;
}