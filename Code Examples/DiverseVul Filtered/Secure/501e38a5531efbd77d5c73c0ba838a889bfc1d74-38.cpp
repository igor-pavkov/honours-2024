static long ep_ioctl(struct file *fd, unsigned code, unsigned long value)
{
	struct ep_data		*data = fd->private_data;
	int			status;

	if ((status = get_ready_ep (fd->f_flags, data, false)) < 0)
		return status;

	spin_lock_irq (&data->dev->lock);
	if (likely (data->ep != NULL)) {
		switch (code) {
		case GADGETFS_FIFO_STATUS:
			status = usb_ep_fifo_status (data->ep);
			break;
		case GADGETFS_FIFO_FLUSH:
			usb_ep_fifo_flush (data->ep);
			break;
		case GADGETFS_CLEAR_HALT:
			status = usb_ep_clear_halt (data->ep);
			break;
		default:
			status = -ENOTTY;
		}
	} else
		status = -ENODEV;
	spin_unlock_irq (&data->dev->lock);
	mutex_unlock(&data->lock);
	return status;
}