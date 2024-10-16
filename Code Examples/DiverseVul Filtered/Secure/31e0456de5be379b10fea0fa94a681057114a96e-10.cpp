static void do_submit_urb(struct work_struct *work)
{
	struct smsusb_urb_t *surb = container_of(work, struct smsusb_urb_t, wq);
	struct smsusb_device_t *dev = surb->dev;

	smsusb_submit_urb(dev, surb);
}