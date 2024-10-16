static int uvc_resume(struct usb_interface *intf)
{
	struct uvc_device *dev = usb_get_intfdata(intf);
	int ret;

	uvc_trace(UVC_TRACE_SUSPEND, "Resuming interface %u\n",
		intf->cur_altsetting->desc.bInterfaceNumber);

	if (intf->cur_altsetting->desc.bInterfaceSubClass == SC_VIDEOCONTROL) {
		if ((ret = uvc_ctrl_resume_device(dev)) < 0)
			return ret;

		return uvc_status_resume(dev);
	}

	if (dev->video.streaming->intf != intf) {
		uvc_trace(UVC_TRACE_SUSPEND, "Resume: video streaming USB "
				"interface mismatch.\n");
		return -EINVAL;
	}

	return uvc_video_resume(&dev->video);
}