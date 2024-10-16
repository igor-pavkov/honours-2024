static void sisusb_bulk_completein(struct urb *urb)
{
	struct sisusb_usb_data *sisusb = urb->context;

	if (!sisusb || !sisusb->sisusb_dev || !sisusb->present)
		return;

	sisusb->completein = 1;
	wake_up(&sisusb->wait_q);
}