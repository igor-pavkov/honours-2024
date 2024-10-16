static inline void megasas_remove_scsi_device(struct scsi_device *sdev)
{
	sdev_printk(KERN_INFO, sdev, "SCSI device is removed\n");
	scsi_remove_device(sdev);
	scsi_device_put(sdev);
}