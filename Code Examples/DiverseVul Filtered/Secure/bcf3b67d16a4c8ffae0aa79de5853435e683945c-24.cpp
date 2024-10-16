static int megasas_reset_bus_host(struct scsi_cmnd *scmd)
{
	int ret;
	struct megasas_instance *instance;

	instance = (struct megasas_instance *)scmd->device->host->hostdata;

	scmd_printk(KERN_INFO, scmd,
		"Controller reset is requested due to IO timeout\n"
		"SCSI command pointer: (%p)\t SCSI host state: %d\t"
		" SCSI host busy: %d\t FW outstanding: %d\n",
		scmd, scmd->device->host->shost_state,
		scsi_host_busy(scmd->device->host),
		atomic_read(&instance->fw_outstanding));

	/*
	 * First wait for all commands to complete
	 */
	if (instance->adapter_type == MFI_SERIES) {
		ret = megasas_generic_reset(scmd);
	} else {
		struct megasas_cmd_fusion *cmd;
		cmd = (struct megasas_cmd_fusion *)scmd->SCp.ptr;
		if (cmd)
			megasas_dump_frame(cmd->io_request,
				MEGA_MPI2_RAID_DEFAULT_IO_FRAME_SIZE);
		ret = megasas_reset_fusion(scmd->device->host,
				SCSIIO_TIMEOUT_OCR);
	}

	return ret;
}