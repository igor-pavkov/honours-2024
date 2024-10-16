static int usbvision_v4l2_close(struct file *file)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	PDEBUG(DBG_IO, "close");

	mutex_lock(&usbvision->v4l2_lock);
	usbvision_audio_off(usbvision);
	usbvision_restart_isoc(usbvision);
	usbvision_stop_isoc(usbvision);

	usbvision_decompress_free(usbvision);
	usbvision_frames_free(usbvision);
	usbvision_empty_framequeues(usbvision);
	usbvision_scratch_free(usbvision);

	usbvision->user--;
	mutex_unlock(&usbvision->v4l2_lock);

	if (usbvision->remove_pending) {
		printk(KERN_INFO "%s: Final disconnect\n", __func__);
		usbvision_release(usbvision);
		return 0;
	}

	PDEBUG(DBG_IO, "success");
	return v4l2_fh_release(file);
}