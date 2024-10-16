static void megasas_slave_destroy(struct scsi_device *sdev)
{
	kfree(sdev->hostdata);
	sdev->hostdata = NULL;
}