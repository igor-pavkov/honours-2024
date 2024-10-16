static int uvc_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct uvc_device *dev = usb_get_intfdata(intf);

	uvc_trace(UVC_TRACE_SUSPEND, "Suspending interface %u\n",
		intf->cur_altsetting->desc.bInterfaceNumber);

	/* Controls are cached on the fly so they don't need to be saved. */
	if (intf->cur_altsetting->desc.bInterfaceSubClass == SC_VIDEOCONTROL)
		return uvc_status_suspend(dev);

	if (dev->video.streaming->intf != intf) {
		uvc_trace(UVC_TRACE_SUSPEND, "Suspend: video streaming USB "
				"interface mismatch.\n");
		return -EINVAL;
	}

	return uvc_video_suspend(&dev->video);
}