static void sisusb_bulk_completeout(struct urb *urb)
{
	struct sisusb_urb_context *context = urb->context;
	struct sisusb_usb_data *sisusb;

	if (!context)
		return;

	sisusb = context->sisusb;

	if (!sisusb || !sisusb->sisusb_dev || !sisusb->present)
		return;

#ifndef SISUSB_DONTSYNC
	if (context->actual_length)
		*(context->actual_length) += urb->actual_length;
#endif

	sisusb->urbstatus[context->urbindex] &= ~SU_URB_BUSY;
	wake_up(&sisusb->wait_q);
}